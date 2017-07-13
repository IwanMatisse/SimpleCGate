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

/*
It is a base for communication with the extern GUI. 
It stores entities from the GateDatabase in local containers 
that can be used for safe transmission (without any locks on the main threads) to other application.
*/

class DataServer:public IEventConsumer
{
public:

	DataServer(EventManager& manager, GateDatabase& db);
	virtual ~DataServer()
	{
		event_manager_.RemoveConsumer(this);
	}

	DataServer(const DataServer&) = delete;
	DataServer& operator=(const DataServer&) = delete;
private:	

	virtual void EventProcess(const Event& _event) override;
	EventManager& event_manager_;

	std::vector<std::shared_ptr<Trade>> my_trades_;
	std::mutex my_trades_lock_;
	std::vector<std::shared_ptr<Order >> orders_;
	std::mutex orders_lock_;
	std::vector<std::shared_ptr<Security>> securities_;
	std::mutex securities_lock_;
	std::vector<std::shared_ptr<Position>> positions_;
	std::mutex positions_lock_;
	std::vector<std::shared_ptr<Strategy>> Strategies_;
	std::mutex strategies_lock_;

	std::shared_ptr<Money> money_info_;
	GateDatabase& database_;


};