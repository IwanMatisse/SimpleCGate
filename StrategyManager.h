#pragma once
#pragma once
#include "stdafx.h"
#include "EventManager.h"
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <set>
#include <map>
#include <atomic>
#include <thread>
#include "Strategy.h"
#include "Security.h"
#include "Order.h"
#include "Trade.h"
#include "PlazaConnector.h"


/*
The StrategyManager controls a pool of strategies in two threads: the main thread and the background thread.
All the events that were received from other subsystems process in the main thread. 
Types of events: New security, New trade, Changing order, Changing price. 
Each strategy has to be subscribed on the changing prices only in used securities. 
It can be done by calling StrategyManager::AddStrategy(<strategy>, <list of securities>)
*/

class StrategyManager:public IEventConsumer
{	
public:
	virtual void BackgroundProcess();
	
	virtual void AddStrategy(const std::shared_ptr<Strategy>& strategy, std::vector<std::shared_ptr<Security>> securities);
	StrategyManager(EventManager& eve, GateDatabase& db);
	virtual ~StrategyManager();
	virtual void OnDisconnect();

	StrategyManager(const StrategyManager&) = delete;
	StrategyManager& operator=(const StrategyManager&) = delete;

private:
	
	std::thread background_thread_;
	std::atomic<bool> exit_flag_ = true;

	bool is_connected_{ false };

	GateDatabase& database_;

	virtual void EventProcess(const Event& _event) override;

	EventManager& event_manager_;
	std::mutex algo_lock_;
	std::vector<std::shared_ptr<Strategy>> strategies_;

	std::map<int, std::vector<std::shared_ptr<Strategy>>> strategies_map_; //link between a security and strategies using the security
};