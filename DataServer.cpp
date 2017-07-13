#include "stdafx.h"
#include "DataServer.h"



DataServer::DataServer(EventManager& manager, GateDatabase &db) :
	event_manager_{ manager }, 
	database_{ db }	
{
	
	event_manager_.AddNewConsumer(this);
	
}


void DataServer::EventProcess(const Event& _event) {
	switch (_event.type)
	{
	case Event::Types::kSecurityAdd:
	{
		if (_event.global_id == 0) break;
		auto sec_ptr = database_.GetSecurityByGlobalId(_event.global_id);		
		std::lock_guard<std::mutex> locker(securities_lock_);
		securities_.push_back(sec_ptr);
		break;
	}
	case Event::Types::kNewStrategy:
	{
		if (_event.global_id == 0) break;
		auto str = database_.GetStrategyByGlobalId(_event.global_id);
		//Strategy * algo = static_cast<Strategy*>(_event.data);
		std::lock_guard<std::mutex> locker(strategies_lock_);
		Strategies_.push_back(str);
		break;
	}
	case Event::Types::kMyTradeAdd:
	{
		if (_event.global_id == 0) break;
		auto deal = database_.GetTradeByGlobalId(_event.global_id);
		std::lock_guard<std::mutex> locker(my_trades_lock_);
		my_trades_.push_back(deal);
		break;
	}
	case Event::Types::kQuoteChange:
	{
		break;
	}
	case Event::Types::kMoneyChange:
	{		
		money_info_ = database_.money_info();		
		break;
	}
	case Event::Types::kPositionClear:
	{
		std::lock_guard<std::mutex> locker(positions_lock_);
		positions_.clear();
		break;
	}
	case Event::Types::kPositionChange:
	{
		if (_event.global_id == 0) break;
		auto event_pos = database_.GetPositionByGlobalId(_event.global_id);
		std::lock_guard<std::mutex> locker(positions_lock_);

		for (auto& data_pos: positions_)
		{
			if (data_pos->global_id() == event_pos->global_id())
			{
				data_pos->Fill(*event_pos);
				return;
			}
		}
		positions_.push_back(event_pos);
		break;
	}
	case Event::Types::kOrderChange:
	{
		if (_event.global_id == 0) break;
		auto ord = database_.GetOrderByGlobalId(_event.global_id);
		std::lock_guard<std::mutex> locker(orders_lock_);
		orders_.push_back(ord);
		break;
	}
	}

}
