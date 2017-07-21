

#pragma once
#include "stdafx.h"
#include "TransactionManager.h"
#include "PlazaConnector.h"
#include <map>
#include "Utils.h"
#include "windows.h"

namespace simple_cgate
{
	CG_RESULT TransactionManager::CallbackWrapper(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg, void* data)
	{
		auto tr = static_cast<PlazaConnector*>(data);
		return tr->PublisherCallback(conn, listener, msg);
	}

	void TransactionManager::TransactionProcess()
	{
		uint32_t state = CG_STATE_CLOSED;
		uint32_t state_lsn = CG_STATE_CLOSED;
		int last_log = 0, last_logl = 0;

		while (!exit_)
		{
			try
			{
				// query connection state
				cg_conn_getstate(connection_, &state);

				if (state == CG_STATE_ERROR)
				{
					state_ = DataStreamState::kError;
					printf("PlazaConnector: TransactionManager connection in error state. \n");

					cg_conn_close(connection_);
				}
				else
					if (state == CG_STATE_CLOSED)
					{
						state_ = DataStreamState::kClosed;
						printf("PlazaConnector: TransactionManager stream is CLOSED. \n");
						cg_conn_open(connection_, 0);
					}
					else if (state == CG_STATE_ACTIVE)
					{
						state_ = DataStreamState::kActive;
						uint32_t result = cg_conn_process(connection_, 1, 0);
						if (result == CG_ERR_OK)
							for (int i = 0; ((i < 100) && (cg_conn_process(connection_, 0, 0) == CG_ERR_OK)); i++);

						if (result != CG_ERR_OK && result != CG_ERR_TIMEOUT)
						{
							printf("PlazaConnector: TransactionManager conn_process is in ACTIVE state. Coonnection state request failed. \n");
						}

						if (listener_ != nullptr)
						{
							cg_lsn_getstate(listener_, &state_lsn);
							switch (state_lsn)
							{
							case CG_STATE_CLOSED:
							{
								cg_lsn_open(listener_, 0);
								break;
							}
							case CG_STATE_ERROR: cg_lsn_close(listener_);
								break;
							}
						}

						if (publisher_ != nullptr)
						{
							cg_pub_getstate(publisher_, &state_lsn);
							switch (state_lsn)
							{
							case CG_STATE_CLOSED:
							{
								cg_pub_open(publisher_, 0);
								break;
							}
							case CG_STATE_ERROR: cg_pub_close(publisher_);
								break;
							}
						}
					}
			}
			catch (...) {
				printf("PlazaConnector: Something is wrong \n");
			}
		}
		return;
	}

	void TransactionManager::PublisherProcess()
	{
		while (!exit_)
		{
			std::unique_lock<std::mutex> locker(queuelock_);
			queuecheck_.wait(locker, [&]() {return !transactions_.empty(); });

			while (!transactions_.empty())
			{
				if (state_ == DataStreamState::kActive)
				{
					Transaction tr = transactions_.front();
					transactions_.pop();
					if (tr.order_main == nullptr || tr.type == TransactionType::kNone) continue;

					switch (tr.type)
					{
					case TransactionType::kAddOrder:
						PostNewOrder(tr.order_main);
						break;
					case TransactionType::kCancelOrder:
						PostCancelOrder(tr.order_main);
						break;
					case TransactionType::kMoveOrder:
						PostMoveOrder(tr.order_main, tr.order_second);
						break;
					}

				}
			}
		}
	}


	void TransactionManager::StartPublisher(const std::string& conn_str, const std::string& broker_code, const std::string& client_code)
	{
		if (broker_code_ == nullptr)
		{
			broker_code_ = new char[5];
			memcpy(broker_code_, broker_code.c_str(), 5 * sizeof(char));
		}
		if (client_code_ == nullptr)
		{
			client_code_ = new char[4];
			memcpy(client_code_, client_code.c_str(), 4 * sizeof(char));
		}
		cg_conn_new((conn_str + 'P').c_str(), &connection_);
		if (connection_ == NULL)
		{
			printf("Cannot create a connection. Maybe something is wrong with config file.\n");
			throw "Creating connection failed";
		}
		cg_pub_new(connection_, "p2mq://FORTS_SRV;category=FORTS_MSG;name=srvlink;timeout=5000;scheme=|FILE|INI/forts_messages.ini|message", &publisher_);
		cg_lsn_new(connection_, "p2mqreply://;ref=srvlink", &CallbackWrapper, plaza_connector_, &listener_);


		exit_ = false;
		transaction_thread_ = std::move(std::thread(&TransactionManager::TransactionProcess, this));
		publisher_thread_ = std::move(std::thread(&TransactionManager::PublisherProcess, this));
	}

	void TransactionManager::Stop()
	{
		exit_ = true;

		{
			std::unique_lock<std::mutex> locker(queuelock_);
			transactions_.push(std::move(Transaction(TransactionType::kNone, nullptr)));
			queuecheck_.notify_one();
		}

		transaction_thread_.join();
		publisher_thread_.join();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (listener_ != nullptr)
		{
			cg_lsn_close(listener_);
			cg_lsn_destroy(listener_);
		}
		listener_ = nullptr;


		if (publisher_ != nullptr)
			cg_pub_destroy(publisher_);
		publisher_ = nullptr;

		if (connection_ != nullptr)
			cg_conn_destroy(connection_);
		connection_ = nullptr;

	}

	TransactionManager::TransactionManager(void *_plaza)
	{
		exit_ = true;

		plaza_connector_ = _plaza;
		broker_code_ = nullptr;
		client_code_ = nullptr;
	}


	TransactionManager::~TransactionManager()
	{
		if (exit_ == false)
		{
			Stop();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		if (client_code_ != nullptr)
			delete client_code_;
		if (broker_code_ != nullptr)
			delete broker_code_;
	}


	void TransactionManager::PostCancelOrder(Order* order)
	{
		struct cg_msg_data_t* msg = nullptr;

		cg_pub_msgnew(publisher_, CG_KEY_NAME, order->security()->type() == SecurityType::kOption ? "OptDelOrder" : "FutDelOrder", (struct cg_msg_t**)&(msg));
		struct plaza::FutDelOrder* del_msg = (struct plaza::FutDelOrder*)msg->data;
		strcpy_s(del_msg->broker_code, broker_code_);

		msg->user_id = order->user_id();
		del_msg = (struct plaza::FutDelOrder*)msg->data;
		del_msg->order_id = order->id();

		if (msg != nullptr)
		{
			cg_pub_post(publisher_, (struct cg_msg_t *)msg, CG_PUB_NEEDREPLY);
			cg_pub_msgfree(publisher_, (struct cg_msg_t *)msg);
		}
	}

	void TransactionManager::PostMoveOrder(Order* old_order, Order* new_order)
	{
		struct cg_msg_data_t* msg = nullptr;
		cg_pub_msgnew(publisher_, CG_KEY_NAME, "FutMoveOrder", (struct cg_msg_t**)&(msg));

		struct plaza::FutMoveOrder* move_msg = (struct plaza::FutMoveOrder*)msg->data;
		strcpy_s(move_msg->broker_code, broker_code_);

		msg->user_id = new_order->user_id();
		move_msg = (struct plaza::FutMoveOrder*)msg->data;
		move_msg->regime = 1;
		move_msg->amount1 = new_order->volume();
		move_msg->ext_id1 = new_order->user_id();
		move_msg->order_id1 = old_order->id();

		new_order->security()->ConvertPriceToStr(new_order->price(), move_msg->price1);

		if (msg != nullptr)
		{
			cg_pub_post(publisher_, (struct cg_msg_t *)msg, CG_PUB_NEEDREPLY);
			cg_pub_msgfree(publisher_, (struct cg_msg_t *)msg);
		}

	}
	void TransactionManager::PostNewOrder(Order* order)
	{
		struct cg_msg_t *msg;
		if (order->volume() <= 0)
		{
			order->SetState(OrderState::kFailed);
			return;
		}
		if (order->security()->type() == SecurityType::kSpread)
		{
			msg = PrepareMultilegOrder(order);
		}
		else
			msg = PrepareFutAddOrder(order);

		if (msg != nullptr)
		{
			cg_pub_post(publisher_, (struct cg_msg_t *)msg, CG_PUB_NEEDREPLY);
			cg_pub_msgfree(publisher_, (struct cg_msg_t *)msg);
		}
	}

	struct cg_msg_t * TransactionManager::PrepareFutAddOrder(Order* order)
	{
		struct plaza::_FutAddOrder* ord;
		struct cg_msg_data_t* msg = nullptr;

		cg_pub_msgnew(publisher_, CG_KEY_NAME, order->security()->type() == SecurityType::kOption ? "OptAddOrder" : "FutAddOrder", (struct cg_msg_t**)&msg);


		ord = (struct plaza::_FutAddOrder*)msg->data;
		strcpy_s(ord->broker_code, broker_code_);
		strcpy_s(ord->isin, order->security()->isin().c_str());
		strcpy_s(ord->client_code, client_code_);
		msg->user_id = order->user_id();

		ord = (struct plaza::_FutAddOrder*)msg->data;
		ord->type = order->type();
		ord->dir = order->direction();
		ord->amount = order->volume();
		order->security()->ConvertPriceToStr(order->price(), ord->price);
		ord->ext_id = order->user_id();

		return (struct cg_msg_t *)msg;

	}
	struct cg_msg_t * TransactionManager::PrepareMultilegOrder(Order* order)
	{
		struct plaza::FutAddMultiLegOrder* ord;
		struct cg_msg_data_t* msg = nullptr;
		cg_pub_msgnew(publisher_, CG_KEY_NAME, "FutAddMultiLegOrder", (struct cg_msg_t**)&msg);

		ord = (struct plaza::FutAddMultiLegOrder*)msg->data;
		strcpy_s(ord->broker_code, broker_code_);
		ord->trade_mode = 2;
		strcpy_s(ord->client_code, client_code_);

		msg->user_id = order->user_id();
		ord = (struct plaza::FutAddMultiLegOrder*)msg->data;

		ord->type = order->type();
		ord->dir = order->direction();
		ord->amount = order->volume();

		ord->sess_id = order->security()->sess_id();
		ord->isin_id = order->security()->isin_id();

		order->security()->ConvertPriceToStr(order->price(), ord->rate_price);
		strcpy_s(ord->price, ord->rate_price);
		ord->ext_id = order->user_id();

		return (struct cg_msg_t *)msg;
	}

	void TransactionManager::AddOrder(Order * order)
	{
		std::unique_lock<std::mutex> locker(queuelock_);
		transactions_.push(std::move(Transaction(TransactionType::kAddOrder, order)));
		queuecheck_.notify_one();
	}

	void TransactionManager::CancelOrder(Order * order)
	{
		std::unique_lock<std::mutex> locker(queuelock_);
		transactions_.push(std::move(Transaction(TransactionType::kCancelOrder, order)));
		queuecheck_.notify_one();
	}

	void TransactionManager::MoveOrder(Order* old_order, Order* new_order)
	{
		if (new_order->direction() != old_order->direction())
		{
			CancelOrder(old_order);
			AddOrder(new_order);
		}
		else
		{
			std::unique_lock<std::mutex> locker(queuelock_);
			transactions_.push(std::move(Transaction(TransactionType::kMoveOrder, old_order, new_order)));
			queuecheck_.notify_one();
		}
	}
}// namespace simple_cgate