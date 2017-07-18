#pragma once

#include "stdafx.h"
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include "EventConsumer.h"

/*

The EventsManager provides communication between all other parts of the application. Mediator pattern.
The instance of this class has to be created before the StrategyManager, the DataServer, and the PlazaConnector.
These subsystems add themselves into the EventsManager in their constructors.
*/
namespace simple_cgate
{
	class EventManager
	{
	public:
		virtual void AddNewConsumer(IEventConsumer* const cons);
		virtual void RemoveConsumer(const IEventConsumer* cons);
		virtual void AddEvent(Event::Types type, int global_id);
		virtual void AddEvent(Event::Types type);
		virtual void AddCommandEvent(const Command& com);

		EventManager() = default;
		virtual ~EventManager() = default;
		EventManager(const EventManager&) = delete;
		EventManager& operator=(const EventManager&) = delete;
	private:

		std::mutex lock_;
		std::vector<IEventConsumer*> consumers_;


	};
}// namespace simple_cgate
