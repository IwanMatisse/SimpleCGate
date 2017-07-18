
#include "stdafx.h"
#include "EventManager.h"


namespace simple_cgate
{
	void EventManager::AddNewConsumer(IEventConsumer* const cons)
	{
		std::lock_guard<std::mutex> locker(lock_);
		consumers_.push_back(cons);
	}

	void EventManager::RemoveConsumer(const IEventConsumer* cons)
	{
		std::lock_guard<std::mutex> locker(lock_);
		for (auto it = consumers_.begin(); it != consumers_.end(); ++it)
		{
			if ((*it)->event_consumer_id() == cons->event_consumer_id())
			{
				consumers_.erase(it);
				return;
			}
		}
	}

	void EventManager::AddEvent(Event::Types type)
	{
		std::lock_guard<std::mutex> locker(lock_);
		for (auto consumer : consumers_)
		{
			consumer->Notify(Event(type, 0));
		}
	}

	void EventManager::AddEvent(Event::Types type, int global_id)
	{
		std::lock_guard<std::mutex> locker(lock_);
		for (auto consumer : consumers_)
		{
			consumer->Notify(Event(type, global_id));
		}
	}

	void EventManager::AddCommandEvent(const Command& com)
	{
		std::lock_guard<std::mutex> locker(lock_);
		for (auto consumer : consumers_)
		{
			consumer->Notify(Event(com));
		}
	}
}//namespace simple_cgate