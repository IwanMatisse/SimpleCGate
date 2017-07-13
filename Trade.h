#pragma once

#include "stdafx.h"
#include "Security.h"
#include "Order.h"
#include "Entity.h"


class Trade:public Entity
{
public:

	Trade(const std::shared_ptr<Security>& security, const std::shared_ptr<Order>& order);
	Trade(const unsigned long long id, const std::shared_ptr<Security>& security, const std::shared_ptr<Order>& order);
	Trade(const Trade&) = delete;
	Trade& operator=(const Trade&) = delete;
	virtual ~Trade() = default;
	virtual void AddTrade(const std::shared_ptr<Trade>& add);//Trade1.AddTrade(Trade2) = Trade1.volume+=Trade2.volume, Trade1.price = avg(Trade1.price, Trade2.price) 

	virtual const std::shared_ptr<Security>& security() const;
	virtual TradeDirection direction() const;
	virtual double price() const;
	virtual int volume() const;
	virtual unsigned long long id() const;
	virtual const std::shared_ptr<Order>& order() const;
	virtual const DateTime& time();

	virtual void SetPrice(const double price);
	virtual void SetTime(const DateTime& time);
	virtual void SetVolume(const int volume);
	virtual void SetId(const unsigned long long id);
	virtual void SetDirection(const TradeDirection direction);

private:
	const std::shared_ptr<Security> security_;
	TradeDirection direction_{TradeDirection::kBuy};
	
	double price_{0.0};
	int volume_{0};
	unsigned long long id_{0};
	const std::shared_ptr<Order> order_;
	DateTime time_;	
};
