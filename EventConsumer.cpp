#include "stdafx.h"
#include "EventConsumer.h"

int IEventConsumer::sEventConsumerCounter;

void IEventConsumer::Notify(const Event& ev)
{	
	{
		std::unique_lock<std::mutex> locker(lock_ec_);
		events_ec_.push(ev);
	}
	check_ec_.notify_one();	
}

/*void EventConsumer::SetId(const int id)
{
	id_ = id;
}*/
int IEventConsumer::event_consumer_id() const
{
	return event_consumer_id_;
}

IEventConsumer::IEventConsumer():
	event_consumer_id_{ sEventConsumerCounter++ }
{		
	exit_flag_ec_ = false;
	event_consumer_thread_ = std::move(std::thread(&IEventConsumer::ThreadFunction, this));
}
IEventConsumer::~IEventConsumer()
{
	exit_flag_ec_.store(true);
	Notify(Event(Event::Types::kNone, 0));
	using namespace std::chrono_literals;
	
	std::this_thread::sleep_for(10ms);
	if (event_consumer_thread_.joinable())
		event_consumer_thread_.join();
}



void IEventConsumer::ThreadFunction()
{
	while (!exit_flag_ec_)
	{
		WaitCondition();
		if (exit_flag_ec_)
			return;
		while (!events_ec_.empty())
		{
			Event _event(Event::Types::kNone, 0);
			{
				std::unique_lock<std::mutex> locker(lock_ec_);
				_event = events_ec_.front();
				events_ec_.pop();
			}
			EventProcess(_event);
		}
	}
}

void IEventConsumer::WaitCondition()
{
	std::unique_lock<std::mutex> locker(lock_ec_);
	check_ec_.wait(locker, [&]() {return (!events_ec_.empty() || exit_flag_ec_ == true); });
}