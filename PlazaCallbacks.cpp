
#include "stdafx.h"
#include "PlazaConnector.h"
#include "PlazaStructures.h"

namespace simple_cgate
{
	void PlazaConnector::ListenerStateProcess(const DataStream& stream, cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg)
	{
		for (auto plaza_stream : stream.GetPlazaStreams())
		{
			if (plaza_stream.listener == listener)
			{
				switch (msg->type)
				{
				case CG_MSG_P2REPL_ONLINE:
				{
					printf("Plaza stream [%s] is ONLINE \n", plaza_stream.name.c_str());
					plaza_stream.state = ListenerState::kOnline;
					break;
				}
				case CG_MSG_OPEN:

					plaza_stream.state = ListenerState::kOpen;
					break;
				case CG_MSG_CLOSE:
					plaza_stream.state = ListenerState::kClose;
					break;

				case CG_MSG_P2REPL_LIFENUM:
					// Stream data scheme's life number was changed.
					// complete data snapshot re-replication will occur.				
					plaza_stream.life_num = *((uint32_t*)msg->data);
					break;
				case CG_MSG_P2REPL_REPLSTATE:
					// Datastream state is recevied to be used for resume later.
					// Message content may be stored anywhere as a string
					// and then used in cg_lsn_open(..) call as value for
					// parameter "replstate="		

					plaza_stream.replica_state = std::string(static_cast<char*>(msg->data));
					break;
				}
			}

		}
	}


	CG_RESULT PlazaConnector::ListenerPartStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg)
	{

		switch (msg->type)
		{
		case CG_MSG_STREAM_DATA:
		{
			struct cg_msg_streamdata_t* replmsg = (struct cg_msg_streamdata_t*)msg;
			if (replmsg->msg_index == 0)
			{
				plaza::part* part = (plaza::part*)replmsg->data;
				auto money = database_.money_info();

				money->SetCoeffGO(ConvertToDouble(part->coeff_go));

				money->SetAll(ConvertToDouble(part->money_amount));
				money->SetFree(ConvertToDouble(part->money_free));
				money->SetBlocked(ConvertToDouble(part->money_blocked));
				money->SetFee(ConvertToDouble(part->fee));
				AddMoneyEvent(money);
			}
			break;
		}
		case CG_MSG_P2REPL_CLEARDELETED:		break;
		case CG_MSG_TN_BEGIN:					break;
		case CG_MSG_TN_COMMIT:					break;
		case CG_MSG_P2REPL_ONLINE:
			printf("PART ONLINE. Money: %f\n", (double)database_.money_info()->free_amount());
		default:
			ListenerStateProcess(service_stream_, conn, listener, msg);
			break;
		}
		return CG_ERR_OK;
	}

	CG_RESULT PlazaConnector::ListenerFutInfoStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg)
	{

		switch (msg->type)
		{
		case CG_MSG_STREAM_DATA:
		{
			struct cg_msg_streamdata_t* replmsg = (struct cg_msg_streamdata_t*)msg;

			if (replmsg->msg_index == 0)
			{

				plaza::fut_sess_contents* plaza_data = (plaza::fut_sess_contents*)replmsg->data;
				if (plaza_data->sess_id < last_sess_id)
					break; //skip information by previous session

				last_sess_id = plaza_data->sess_id;
				bool is_new = false;

				auto security = database_.IsSecurityExist(plaza_data->isin) ?
					database_.GetSecurity(plaza_data->isin) :
					(is_new = true, database_.CreateSecurity(plaza_data->isin));

				if (IsSecuirtyExist(plaza_data->isin_id) == false)
					AddSecurity(security, plaza_data->isin_id);


				security->SetLastPrice(ConvertToDouble(plaza_data->last_cl_quote));
				security->SetStepPrice(ConvertToDouble(plaza_data->step_price));
				security->SetMarginBuy(ConvertToDouble(plaza_data->buy_deposit));
				security->SetMarginSell(ConvertToDouble(plaza_data->sell_deposit));

				security->SetMinPrice(security->last_price() - ConvertToDouble(plaza_data->limit_down));
				security->SetMaxPrice(security->last_price() + ConvertToDouble(plaza_data->limit_up));
				security->SetDecimals(plaza_data->roundto);
				security->SetTradingState(plaza_data->state == 1);
				security->SetStepSize(ConvertToDouble(plaza_data->min_step));

				if ((int)plaza_data->multileg_type == (int)3)
					security->SetType(SecurityType::kSpread);
				else
					security->SetType(SecurityType::kFutures);

				if (is_new)
					AddSecurityEvent(security);
			}
			break;
		}
		case CG_MSG_P2REPL_CLEARDELETED:		break;
		case CG_MSG_TN_BEGIN:					break;
		case CG_MSG_TN_COMMIT:					break;
		case CG_MSG_CLOSE:
		{
			ListenerStateProcess(service_stream_, conn, listener, msg);
			break;
		}
		case CG_MSG_P2REPL_ONLINE:
		{
			printf("FutInfo ONLINE. Securities length: %d \n", 1);
			for (size_t i = 0; i < service_stream_.GetPlazaStreams().size(); i++)
			{
				service_stream_.SetWaitForFlag(false, static_cast<int>(i));
			}
		}

		default:
			ListenerStateProcess(service_stream_, conn, listener, msg);
			break;
		}
		return CG_ERR_OK;
	}


	CG_RESULT PlazaConnector::ListenerFutCommonStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg)
	{

		switch (msg->type)
		{
		case CG_MSG_STREAM_DATA:
		{
			struct cg_msg_streamdata_t* replmsg = (struct cg_msg_streamdata_t*)msg;

			if (replmsg->msg_index == 0)
			{

				plaza::fut_common* plaza_common = static_cast<plaza::fut_common*>(replmsg->data);

				if (!IsSecuirtyExist(plaza_common->isin_id))
					break;
				auto security = GetSecurityById(plaza_common->isin_id);

				security->SetAsk(ConvertToDouble(plaza_common->best_sell),
					plaza_common->amount_sell);
				security->SetBid(ConvertToDouble(plaza_common->best_buy),
					plaza_common->amount_buy);
				AddQuoteEvent(security);
			}
			break;
		}
		case CG_MSG_P2REPL_CLEARDELETED:		break;
		case CG_MSG_TN_BEGIN:					break;
		case CG_MSG_TN_COMMIT:					break;
		default:
			ListenerStateProcess(common_stream_, conn, listener, msg);
			break;
		}
		return CG_ERR_OK;
	}

	CG_RESULT PlazaConnector::ListenerVMStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg)
	{

		switch (msg->type)
		{
		case CG_MSG_STREAM_DATA:
		{
			struct cg_msg_streamdata_t* replmsg = (struct cg_msg_streamdata_t*)msg;
			if (replmsg->msg_index == 0)
			{
				plaza::fut_vm* plaza_vm = static_cast<plaza::fut_vm*>(replmsg->data);

				if (!IsSecuirtyExist(plaza_vm->isin_id))
					break;
				auto security = GetSecurityById(plaza_vm->isin_id);

				auto position = database_.IsPositionExist(security->isin()) ?
					database_.GetPosition(security->isin()) :
					database_.CreatePosition(security);

				position->SetVariationMargin(ConvertToDouble(plaza_vm->vm_real));
				AddPositionEvent(position);

			}
			break;
		}
		case CG_MSG_P2REPL_CLEARDELETED:		break;
		case CG_MSG_TN_BEGIN:					break;
		case CG_MSG_TN_COMMIT:					break;
		default:
			ListenerStateProcess(service_stream_, conn, listener, msg);
			break;
		}
		return CG_ERR_OK;
	}

	CG_RESULT PlazaConnector::ListenerPositionStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg)
	{

		switch (msg->type)
		{
		case CG_MSG_STREAM_DATA:
		{
			struct cg_msg_streamdata_t* replmsg = (struct cg_msg_streamdata_t*)msg;
			if (replmsg->msg_index == 0)
			{
				plaza::position* plaza_pos = static_cast<plaza::position*>(replmsg->data);

				if (!IsSecuirtyExist(plaza_pos->isin_id))
					break;
				auto security = GetSecurityById(plaza_pos->isin_id);

				auto position = database_.IsPositionExist(security->isin()) ?
					database_.GetPosition(security->isin()) :
					database_.CreatePosition(security);

				position->Fill(plaza_pos->open_qty,
					plaza_pos->buys_qty,
					plaza_pos->sells_qty,
					plaza_pos->pos,
					ConvertToDouble(plaza_pos->waprice));

				AddPositionEvent(position);
			}
			break;
		}
		case CG_MSG_P2REPL_CLEARDELETED:
		{
			database_.ClearPosition();
			event_manager_.AddEvent(Event::Types::kPositionClear, 0);
			break;
		}
		case CG_MSG_TN_BEGIN:		break;
		case CG_MSG_TN_COMMIT:		break;
		default:
		{
			ListenerStateProcess(service_stream_, conn, listener, msg);
			break;
		}
		}
		return CG_ERR_OK;
	}



	CG_RESULT PlazaConnector::ListenerFutTradeStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg)
	{

		switch (msg->type)
		{
		case CG_MSG_STREAM_DATA:
		{
			struct cg_msg_streamdata_t* replmsg = (struct cg_msg_streamdata_t*)msg;

			switch (replmsg->msg_index)
			{
			case 0://orders
			{
				plaza::my_orders_log *plaza_order = static_cast<plaza::my_orders_log*>(replmsg->data);

				if (!IsSecuirtyExist(plaza_order->isin_id))
					break;
				auto security = GetSecurityById(plaza_order->isin_id);

				auto order = database_.IsOrderExist(plaza_order->ext_id) ?
					database_.GetOrder(plaza_order->ext_id) :
					database_.CreateOrder(plaza_order->ext_id, security);

				switch (plaza_order->action)
				{
				case 0:
				{
					order->SetState(kDone);
					break;
				}
				case 1:
				{
					order->SetDirection(plaza_order->dir == 1 ? kBuy : kSell);
					order->SetPrice(ConvertToDouble(plaza_order->price));
					order->SetBalance(plaza_order->amount);
					order->SetVolume(plaza_order->amount);
					order->SetState(kActive);
					order->SetTime(ConvertToTime(plaza_order->moment));
					break;
				}
				case 2:
				{
					order->SetBalance(plaza_order->amount_rest);
					if (plaza_order->amount_rest == 0)
						order->SetState(kDone);
					break;
				}
				}
				AddOrderEvent(order);

				break;
			}
			case 1: // multileg_order
			{
				plaza::my_multileg_orders_log *plaza_order = (plaza::my_multileg_orders_log*)replmsg->data;

				if (!IsSecuirtyExist(plaza_order->isin_id))
					break;
				auto security = GetSecurityById(plaza_order->isin_id);

				auto order = database_.IsOrderExist(plaza_order->ext_id) ?
					database_.GetOrder(plaza_order->ext_id) :
					database_.CreateOrder(plaza_order->ext_id, security);

				switch (plaza_order->action)
				{
				case 0: {
					order->SetState(kDone);
					break;
				}
				case 1: {
					order->SetDirection(plaza_order->dir == 1 ? kBuy : kSell);
					order->SetPrice(ConvertToDouble(plaza_order->swap_price));

					order->SetBalance(plaza_order->amount);
					order->SetVolume(plaza_order->amount);
					order->SetState(kActive);
					order->SetTime(ConvertToTime(plaza_order->moment));
					break;
				}
				case 2: {
					order->SetBalance(plaza_order->amount_rest);
					if (plaza_order->amount_rest == 0)
						order->SetState(kDone);
					break;
				}
				}

				AddOrderEvent(order);
				break;
			}
			case 2: //multileg_deal
			{
				plaza::multileg_deal *plaza_trade = static_cast<plaza::multileg_deal*>(replmsg->data);
				if (plaza_trade->ext_id_sell == plaza_trade->ext_id_buy)
					break;

				if (database_.IsTradeExist(plaza_trade->id_deal))
					break;

				if (!IsSecuirtyExist(plaza_trade->isin_id))
				{
					break; //have to skip this trade because can not create security by plaza_id only (string isin/name is necessary)
					//to avoid the loss of vital data there are several options:
					//- open fut_sess table first and get all data then open others streams
					//- store skipped trades/orders and process them when the securities come
				}

				auto security = GetSecurityById(plaza_trade->isin_id);
				if (security->type() == SecurityType::kSpread)
				{
					auto userid = plaza_trade->ext_id_buy == 0 ?
						plaza_trade->ext_id_sell :
						plaza_trade->ext_id_buy;

					auto order = database_.IsOrderExist(userid) ?
						database_.GetOrder(userid) :
						database_.CreateOrder(userid, security);

					if (plaza_trade->ext_id_buy == plaza_trade->ext_id_sell)
					{
						break;
					}
					auto my_trade = database_.CreateTrade(plaza_trade->id_deal, security, order);

					if (plaza_trade->ext_id_buy != 0)
					{
						my_trade->SetDirection(kBuy);
					}
					if (plaza_trade->ext_id_sell != 0)
					{
						my_trade->SetDirection(kSell);
					}


					my_trade->SetPrice(ConvertToDouble(plaza_trade->swap_price));
					my_trade->SetVolume(plaza_trade->amount);
					my_trade->SetTime(ConvertToTime(plaza_trade->moment));

					AddMyTradeEvent(my_trade);

				}
				break;
			}
			case 3: //deals
			{
				plaza::user_deal *plaza_trade = static_cast<plaza::user_deal*>(replmsg->data);
				if (plaza_trade->ext_id_sell == plaza_trade->ext_id_buy)
					break;
				if (database_.IsTradeExist(plaza_trade->id_deal))
					break;

				if (!IsSecuirtyExist(plaza_trade->isin_id))
				{
					break; //have to skip this trade because can not create security by plaza_id only (string isin/name is necessary)
						   //to avoid the loss of vital data there are several options:
						   //- open the fut_sess table first and get all data then open others streams
						   //- store the skipped trades/orders and process them when the securities come
				}

				auto security = GetSecurityById(plaza_trade->isin_id);

				auto userid = plaza_trade->ext_id_buy == 0 ?
					plaza_trade->ext_id_sell :
					plaza_trade->ext_id_buy;

				auto order = database_.IsOrderExist(userid) ?
					database_.GetOrder(userid) :
					database_.CreateOrder(userid, security);

				if (plaza_trade->ext_id_buy == plaza_trade->ext_id_sell)
					break;

				auto my_trade = database_.CreateTrade(plaza_trade->id_deal, security, order);

				if (plaza_trade->ext_id_buy != 0)
				{
					my_trade->SetDirection(kBuy);
				}
				if (plaza_trade->ext_id_sell != 0)
				{
					my_trade->SetDirection(kSell);
				}
				my_trade->SetPrice(ConvertToDouble(plaza_trade->price));
				my_trade->SetVolume(plaza_trade->amount);
				my_trade->SetTime(ConvertToTime(plaza_trade->moment));

				AddMyTradeEvent(my_trade);

				break;
			}

			}

			break;
		}
		case CG_MSG_P2REPL_CLEARDELETED: { 	break; }
		case CG_MSG_TN_BEGIN:				break;
		case CG_MSG_TN_COMMIT:
		{
			break;
		}
		default:
			ListenerStateProcess(order_stream_, conn, listener, msg);
			break;
		}
		return CG_ERR_OK;
	}

	CG_RESULT PlazaConnector::PublisherCallback(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg)
	{

		switch (msg->type)
		{
		case CG_MSG_DATA:
		{
			uint32_t user_id = ((struct cg_msg_data_t*)msg)->user_id;
			uint32_t id = ((struct cg_msg_data_t*)msg)->msg_id;
			int code = 0;

			switch (id)
			{
			case 101:
			case 129:
			case 109: //ADD
			{
				code = static_cast<plaza::FORTS_MSG101*>(((cg_msg_data_t*)msg)->data)->code;

				if (database_.IsOrderExist(user_id))
				{
					auto order = database_.GetOrder(user_id);
					if (code == 0)
					{
						order->SetId(((plaza::FORTS_MSG101*)((struct cg_msg_data_t*)msg)->data)->order_id);
					}
					else
					{
						order->SetState(kFailed);
						AddOrderEvent(order);
						printf("ORDER FAILED: %s \n", ((plaza::FORTS_MSG101*)((struct cg_msg_data_t*)msg)->data)->message);
					}
				}

				break;
			}
			case 102:
			case 110:		//DELETE
			{
				code = ((plaza::FORTS_MSG102*)((struct cg_msg_data_t*)msg)->data)->code;
				if (database_.IsOrderExist(user_id))
				{
					auto order = database_.GetOrder(user_id);
					if (code == 0)
					{
						order->SetState(kDone);
					}
					else
					{
						printf("ORDER DELETE FAILED: %s \n", ((plaza::FORTS_MSG102*)((struct cg_msg_data_t*)msg)->data)->message);
					}
				}
				break;
			}
			case 105:
			case 113: //MOVE
			{
				code = ((plaza::FORTS_MSG105*)((struct cg_msg_data_t*)msg)->data)->code;

				if (database_.IsOrderExist(user_id))
				{
					auto order = database_.GetOrder(user_id);

					if (code == 0)
					{
						order->SetId(((plaza::FORTS_MSG105*)((struct cg_msg_data_t*)msg)->data)->order_id1);
						if (order->id() == 0)
						{
							order->SetState(kNone);
							AddOrderEvent(order);
						}
					}
					else
					{
						order->SetState(kFailed);

						AddOrderEvent(order);

						if (code == 50)
						{
							printf("MOVE ORDER FAILED: order for move not found. uid=%d id=%llu \n", order->user_id(), order->id());
						}
						else
							printf("MOVE ORDER FAILED: %d, id1=%llu, id2=%llu \n",
							((plaza::FORTS_MSG105*)((struct cg_msg_data_t*)msg)->data)->code,
								((plaza::FORTS_MSG105*)((struct cg_msg_data_t*)msg)->data)->order_id1,
								((plaza::FORTS_MSG105*)((struct cg_msg_data_t*)msg)->data)->order_id2);

					}
				}

				break;
			}
			case 99://FLOOD
			{
				plaza::FORTS_MSG99* ms = ((plaza::FORTS_MSG99*)((struct cg_msg_data_t*)msg)->data);
				code = 99;
				if (database_.IsOrderExist(user_id))
				{
					auto order = database_.GetOrder(user_id);
					order->SetState(kFailed);
					AddOrderEvent(order);
				}
				break;
			}
			default:
			{
				code = ((plaza::FORTS_MSG101*)((struct cg_msg_data_t*)msg)->data)->code;
			}
			}

			break;
		}
		case CG_MSG_P2MQ_TIMEOUT:
		{
			break;
		}
		}
		return CG_ERR_OK;
	}

}//namespace simple_cgate
