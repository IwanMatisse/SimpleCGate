#include "stdafx.h"
#include "DataServer.h"
#include "DataServer\MessageStructures.h"


DataServer::DataServer(EventManager& manager, GateDatabase &db) :
	event_manager_{ manager },
	database_{ db },
	context{ zmq_ctx_new() },
	incremental_(&DataServer::RequestProcess, this),
	stream_(&DataServer::StreamProcess, this)
{		
	event_manager_.AddNewConsumer(this);	
}

void DataServer::StoreMessage(void *data, int data_size)
{
	zmq_msg_t message;
	zmq_msg_init_size(&message, data_size);
	memcpy(zmq_msg_data(&message), &data, data_size);
	incremental_messages_.push_back(std::move(message));

	srv_data_.all_messages++;
	PublishMessage(&srv_data_, sizeof(srv_data_));
	printf("message: %d \n", srv_data_.all_messages);
}

void DataServer::PublishMessage(void *data, int data_size)
{	
	zmq_msg_t message;
	zmq_msg_init_size(&message, data_size);
	memcpy(zmq_msg_data(&message), &data, data_size);
	stream_messages_.push(std::move(message));
}

void DataServer::StreamProcess()
{
	//clients recieve only actual stream messages; history is not needed

	void* publisher = zmq_socket(context, ZMQ_PUB);
	int conn = zmq_bind(publisher, "tcp://*:5001");

	while (!bExit_)
	{
		while (stream_messages_.empty() != true)
		{
			zmq_msg_t message = stream_messages_.front();
			zmq_msg_send(&message, publisher, 0);
			zmq_msg_close(&message);
			stream_messages_.pop();
		}
	}
	zmq_close(publisher);
}

void DataServer::RequestProcess()
{
	//each client has to recieve all incremental messages

	void* responder = zmq_socket(context, ZMQ_REP);
	int conn = zmq_bind(responder, "tcp://*:5002");
	while (!bExit_)
	{
		zmq_msg_t request;
		zmq_msg_init(&request);
		zmq_msg_recv(&request, responder, 0);//get a number of the message	
		if (bExit_) break;

		auto length = zmq_msg_size(&request);
		if (length != sizeof(client_request))
			throw("Unknown client request structure!");
		client_request req;
		memcpy(&req, zmq_msg_data(&request), length);
		zmq_msg_close(&request);

		if (req.type != data_type::Request)
		{
			throw("Wrong type of incoming message!");
		}

		switch (req.req_type)
		{
		case request_type::Message:
		{
			int req_num = req.value;

			if (req_num <= 0 || req_num > incremental_messages_.size())
			{
				//wrong message's number, send total amount of messages
				zmq_msg_t message;
				zmq_msg_init_size(&message, sizeof(srv_data_));
				memcpy(zmq_msg_data(&message), &srv_data_, sizeof(srv_data_));
				zmq_msg_send(&message, responder, 0);
				zmq_msg_close(&message);
			}
			else
			{
				//send the requested message as replay
				zmq_msg_t message;
				zmq_msg_init(&message);
				zmq_msg_copy(&message, &incremental_messages_[req_num - 1]);
				zmq_msg_send(&message, responder, 0);
				zmq_msg_close(&message);
			}
			break;
		}
		//user's commands:
		case request_type::Connect:
		{

			break;
		}
		case request_type::Disconnect:
		{

			break;
		}
		}
	}
	zmq_close(responder);
}


void DataServer::EventProcess(const Event& _event) {
	switch (_event.type)
	{
	case Event::Types::kQuoteChange:
	case Event::Types::kSecurityAdd:
	{
		if (_event.global_id == 0) break;
		auto sec_ptr = database_.GetSecurityByGlobalId(_event.global_id);		
		
		security_data data;
		//fill the data
		memcpy(data.symbol, sec_ptr->isin().c_str(), min(25, sec_ptr->isin().length()));
		data.symbol[25] = 0;
		data.last_price = sec_ptr->last_price();
		data.id = sec_ptr->global_id();
		data.bid_vol = sec_ptr->bid_volume();
		data.bid = sec_ptr->bid();
		data.ask_vol = sec_ptr->ask_volume();
		data.ask = sec_ptr->ask();

		if (_event.type == Event::Types::kSecurityAdd)
			StoreMessage(&data, sizeof(data));
		else
			PublishMessage(&data, sizeof(data));		
		
		break;
	}
	case Event::Types::kStrategyChange:	
	case Event::Types::kNewStrategy:
	{
		if (_event.global_id == 0) break;
		auto str = database_.GetStrategyByGlobalId(_event.global_id);
		strategy_data data;
		//fill the data
		data.enabled = str->enabled();
		data.price = str->current_price();
		data.state = str->state();
		data.volume = str->current_volume();

		if (_event.type == Event::Types::kNewStrategy)
			StoreMessage(&data, sizeof(data));
		else
			PublishMessage(&data, sizeof(data));

		//todo: strategy change event
		break;
	}
	case Event::Types::kMyTradeAdd:
	{
		if (_event.global_id == 0) break;
		auto deal = database_.GetTradeByGlobalId(_event.global_id);

		trade_data data;
		//fill the data
		data.volume = deal->volume();
		data.symbol_id = deal->security()->global_id();
		data.price = deal->price();
		data.id = deal->global_id();
		data.direction = deal->direction();

		StoreMessage(&data, sizeof(data));		
		break;
	}
	
	case Event::Types::kMoneyChange:
	{		
		auto money_info_ = database_.money_info();	

		money_data data;
		//fill the data
		data.all = money_info_->all_amount();
		data.blocked = money_info_->blocked_amount();
		data.coeffGo = money_info_->coeff_GO();
		data.fee = money_info_->fee();
		data.free = money_info_->free_amount();

		PublishMessage( &data, sizeof(data));
		break;
	}
	case Event::Types::kPositionClear:
	{
		position_clear data;
		PublishMessage(&data, sizeof(data));
		break;
	}
	case Event::Types::kPositionChange:
	{
		if (_event.global_id == 0) break;
		auto event_pos = database_.GetPositionByGlobalId(_event.global_id);

		position_data data;
		//fill the data
		data.vm = event_pos->variation_margin();
		data.symbol_id = event_pos->security()->global_id();
		data.sell = event_pos->sells_amount();
		data.price = event_pos->price();
		data.open = event_pos->open_amount();
		data.current = event_pos->current_amount();
		data.buy = event_pos->buys_amount();

		PublishMessage(&data, sizeof(data));	
		break;
	}
	case Event::Types::kOrderChange:
	{
		if (_event.global_id == 0) break;
		auto ord = database_.GetOrderByGlobalId(_event.global_id);

		order_data data;
		//fill the data
		data.volume = ord->volume();
		data.symbol_id = ord->security()->global_id();
		data.price = ord->price();
		data.id = ord->global_id();
		data.direction = ord->direction();
		data.balance = ord->balance();
		data.state = ord->state();

		StoreMessage(&data, sizeof(data));		
		break;
	}
	case Event::Types::kDisconnect:
	{
		
		break;
	}
	}

}
