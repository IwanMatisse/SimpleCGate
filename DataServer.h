#pragma once
#include "stdafx.h"
#include <string>
#include <algorithm>
#include <iterator>
#include "Trade.h"
#include "Order.h"
#include "Security.h"
#include "Position.h"
#include "Money.h"
#include <vector>
#include <mutex>
#include "EventManager.h"
#include "Strategy.h"
#include "Database.h"
#include "DataServer\MessageStructures.h"

#include <zmq.h>

/*
It is a base for communication with the extern GUI. 
It stores entities from the GateDatabase in local containers 
that can be used for safe transmission (without any locks on the main threads) to other application.
*/

namespace simple_cgate
{

	class DataServer :public IEventConsumer
	{
	public:

		DataServer(EventManager& manager, GateDatabase& db);
		virtual ~DataServer()
		{
			bExit_ = true;
			for (int i = 0; i < incremental_messages_.size(); i++)
			{
				zmq_msg_close(&incremental_messages_[i]);
			}
			zmq_ctx_destroy(context);

			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			event_manager_.RemoveConsumer(this);

			incremental_.join();
			stream_.join();
		}

		DataServer(const DataServer&) = delete;
		DataServer& operator=(const DataServer&) = delete;
	private:

		virtual void EventProcess(const Event& _event) override;
		EventManager& event_manager_;

		void *context;
		std::vector<zmq_msg_t> incremental_messages_;
		std::queue<zmq_msg_t> stream_messages_;
		zmq_msg_t* GetMessageByNum(int num);

		void StoreMessage(void *data, int data_size);
		void PublishMessage(void *data, int data_size);
		void StreamProcess();
		void RequestProcess();

		server_data srv_data_;

		std::thread incremental_;
		std::mutex incremental_lock_;
		std::thread stream_;
		std::mutex stream_lock_;

		GateDatabase& database_;

		bool bExit_ = false;
	};
}