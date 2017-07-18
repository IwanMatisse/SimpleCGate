#pragma once

#include "stdafx.h"
#include <string>
#include <fstream>
#include <time.h>
#include <vector>
#include "DateTime.h"
#include "Entity.h"
#include "Order.h"
#include <functional>


enum AlgoState { OFF = 0, ACTIVE = 1, QUOTING = 2, COVER = 3, WAITWORKTIME = 4, FAILED = 5 };

class Strategy: public Entity
{
public:	
	Strategy(const std::string& name) :name_{name}
	{		
		state_ = AlgoState::OFF;
		enabled_ = false;					
	}
	virtual ~Strategy() = default;
	virtual bool IsTradeTime(const DateTime& time) = 0;
	virtual void ResetStatus() = 0;

	virtual void OnQuotesChange() = 0;
	virtual void OnOrderChange(const int id) = 0;
	virtual void OnNewTrade(const int id) = 0;
	virtual void OnDisconnect() = 0;

	virtual void LoadConfig() = 0;
	virtual void SaveConfig() = 0;

	virtual void OnBackgroundEvent() = 0;
	virtual void OnNewSecurity(const int id) = 0;

	virtual bool enabled()
	{ 
		return enabled_; 
	}
	virtual void SetEnabled(const bool enabled) 
	{ 
		enabled_ = enabled; 
	}
		
	const virtual AlgoState state()
	{
		return state_;
	}

	virtual void SetState(const AlgoState s)
	{
		state_ = s;
	}

	virtual double current_price()
	{
		return 0;
	}

	virtual int current_volume()
	{
		return 0;
	}

	std::function<void(const std::shared_ptr<Order>&)> PostOrder;
	std::function<void(const std::shared_ptr<Order>&, const std::shared_ptr<Order>&)> MoveOrder;
	std::function<void(const std::shared_ptr<Order>&)> CancelOrder;

	virtual std::vector<std::shared_ptr<Security>> GetUsedSecurity() = 0;
private:
	bool enabled_;

protected:
	AlgoState state_;
	std::string name_;

};

