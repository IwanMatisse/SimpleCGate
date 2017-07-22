#pragma once
#include "stdafx.h"
#include <memory>
#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <functional>
#include <thread>
#include <chrono>

/*
To receive and send messages a class need to implement the IEventConsumer interface.
The interface provides processing of messages in a separated thread.
There is only one type of messages. It is Event message. 

*/
namespace simple_cgate
{

	enum CommandType {kCommandConnect = 1, kCommandDisconnect=2, kCommandStart=3, kCommandStop=4};
	//store command's parameters 
	struct Command
	{
		CommandType type;
		int id{ 0 };
		int value{ 0 };

		Command()
		{
		}
		Command(CommandType _com, int _id, int _value)
		{
			type = _com;			
			id = _id;
			value = _value;
		}
	};


	//message for events
	struct Event
	{

		enum class Types {
			kNone, kSecurityAdd, kQuoteChange, kMyTradeAdd, kOrderChange, kMoneyChange, kPositionChange, kPositionClear,
			kCommand, kNewSession, kNewStrategy, kStrategyChange, kDisconnect, kConnect
		};
		Types type;
		Command command; //empty for events
		int global_id; //id of an entity which corresponds with the event.

		Event(const Types _type, const int _global_id) { type = _type; global_id = _global_id; }
		Event(const Command& com) { type = Types::kCommand;  command = com; }
	};


	class IEventConsumer
	{
	public:

		IEventConsumer();
		virtual ~IEventConsumer();
		virtual void Notify(const Event& ev);
		virtual int event_consumer_id() const;
	private:

		std::atomic<bool> exit_flag_ec_;
		std::mutex lock_ec_;
		std::queue<Event> events_ec_;
		std::condition_variable check_ec_;
		std::thread event_consumer_thread_;
		static int sEventConsumerCounter;
		const int event_consumer_id_;
		void WaitCondition();
		virtual void ThreadFunction();
		virtual void EventProcess(const Event& _event) = 0;

		IEventConsumer(const IEventConsumer&) = delete;
		IEventConsumer& operator=(const IEventConsumer&) = delete;

	};

}// namespace simple_cgate