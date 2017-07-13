#include "stdafx.h"
#include "Trade.h"


Trade::Trade(const std::shared_ptr<Security>& security, const std::shared_ptr<Order>& order) :
	security_(security),
	order_(order)
{}
Trade::Trade(const unsigned long long id, const std::shared_ptr<Security>& security, const std::shared_ptr<Order>& order) :
	security_(security),
	order_(order)
{
	id_ = id;
}

void Trade::AddTrade(const std::shared_ptr<Trade>& add)
{
	price_ = (price()*(double)volume() + add->price()*(double)add->volume()) / (double)(volume() + add->volume());
	volume_ = volume() + add->volume();
}

const std::shared_ptr<Security>& Trade::security() const
{
	return security_;
}
TradeDirection Trade::direction() const
{
	return direction_;
}
double Trade::price() const
{
	return price_;
}
int Trade::volume() const
{
	return volume_;
}

unsigned long long Trade::id()const
{
	return id_;
}
const std::shared_ptr<Order>& Trade::order() const
{
	return order_;
}
const DateTime& Trade::time()
{
	return time_;
}

void Trade::SetPrice(const double price)
{
	price_ = price;
}

void Trade::SetTime(const DateTime& time)
{
	time_ = time;
}
void Trade::SetVolume(const int volume)
{
	volume_ = volume;
}
void Trade::SetId(const unsigned long long id)
{
	id_ = id;
}

void Trade::SetDirection(const TradeDirection direction)
{
	direction_ = direction;
}