#include "stdafx.h"
#include "Database.h"
#include <assert.h>

GateDatabase::GateDatabase()
{
	money_info_ = std::make_shared<Money>();
}

std::shared_ptr<Money> GateDatabase::money_info() 
{ 
	return money_info_; 
}

std::shared_ptr<Security> GateDatabase::GetSecurityByGlobalId(const int id) const
{
	return std::static_pointer_cast<Security>(GetEntityByGlobalId(id));
}

std::shared_ptr<Security> GateDatabase::GetSecurity(const std::string name) const
{
	std::lock_guard<std::mutex> locker(securities_lock_);
	auto it = securities_.find(name);
	assert(it != securities_.end() && "You should check existence of the object before trying to get it");
	return it->second;
}

std::shared_ptr<Security> GateDatabase::CreateSecurity(const std::string name)
{
	std::lock_guard<std::mutex> locker(securities_lock_);
	auto& ptr = securities_.insert(std::make_pair(name, std::make_shared<Security>(name))).first->second;
	ptr->SetGlobalId(GetGlobalId());
	entities_[ptr->global_id()] = ptr;
	return ptr;
}

bool GateDatabase::IsSecurityExist(const std::string name)
{
	std::lock_guard<std::mutex> locker(securities_lock_);
	return (securities_.find(name) != securities_.end());
}

std::shared_ptr<Order> GateDatabase::GetOrder(const unsigned int user_id) const
{
	std::lock_guard<std::mutex> locker(orders_lock_);
	auto it = orders_.find(user_id);
	assert(it != orders_.end() && "You should check existence of the object before trying to get it");
	return it->second;
}

std::shared_ptr<Order> GateDatabase::GetOrderByGlobalId(const int id) const
{
	return std::static_pointer_cast<Order>(GetEntityByGlobalId(id));
}

std::shared_ptr<Order> GateDatabase::CreateOrder(const unsigned int user_id, const std::shared_ptr<Security>& sec)
{
	std::lock_guard<std::mutex> locker(orders_lock_);
	auto& ptr = orders_.insert(std::make_pair(user_id, std::make_shared<Order>(sec, user_id))).first->second;
	ptr->SetGlobalId(GetGlobalId());
	entities_[ptr->global_id()] = ptr;
	return ptr;
}
std::shared_ptr<Order> GateDatabase::CreateNewOrder(const std::shared_ptr<Security>& sec)
{
	std::lock_guard<std::mutex> locker(orders_lock_);
	++transaction_id;
	auto& ptr = orders_.insert(std::make_pair(transaction_id, std::make_shared<Order>(sec, transaction_id))).first->second;
	ptr->SetGlobalId(GetGlobalId());
	entities_[ptr->global_id()] = ptr;
	return ptr;
}

bool GateDatabase::IsOrderExist(const unsigned int user_id)
{
	std::lock_guard<std::mutex> locker(orders_lock_);
	return (orders_.find(user_id) != orders_.end());
}

std::shared_ptr<Trade> GateDatabase::GetTradeByGlobalId(const int id) const
{
	return std::static_pointer_cast<Trade>(GetEntityByGlobalId(id));
}

std::shared_ptr<Trade> GateDatabase::GetTrade(const unsigned long long id) const
{
	std::lock_guard<std::mutex> locker(my_trades_lock_);
	auto it = my_trades_.find(id);
	assert(it != my_trades_.end() && "You should check existence of the object before trying to get it");
	return it->second;
}

std::shared_ptr<Trade> GateDatabase::CreateTrade(const unsigned long long id, const std::shared_ptr<Security>& sec, const std::shared_ptr<Order>& ord)
{
	std::lock_guard<std::mutex> locker(my_trades_lock_);
	auto& ptr = my_trades_.insert(std::make_pair(id, std::make_shared<Trade>(id, sec, ord))).first->second;
	ptr->SetGlobalId(GetGlobalId());
	entities_[ptr->global_id()] = ptr;
	return ptr;
}

bool GateDatabase::IsTradeExist(const unsigned long long id)
{
	std::lock_guard<std::mutex> locker(my_trades_lock_);
	return (my_trades_.find(id) != my_trades_.end());
}

std::shared_ptr<Strategy> GateDatabase::AddStrategy(const std::shared_ptr<Strategy>& ptr)
{
	std::lock_guard<std::mutex> locker(entities_lock_);
	ptr->SetGlobalId(GetGlobalId());
	entities_[ptr->global_id()] = ptr;
	return ptr;
}

std::shared_ptr<Strategy> GateDatabase::GetStrategyByGlobalId(const int id) const
{
	return std::static_pointer_cast<Strategy>(GetEntityByGlobalId(id));
}

void GateDatabase::ClearPosition()
{
	std::lock_guard<std::mutex> locker(positions_lock_);
	positions_.clear();
}

std::shared_ptr<Position> GateDatabase::GetPositionByGlobalId(const int id) const
{	
	return std::static_pointer_cast<Position>(GetEntityByGlobalId(id));
}

std::shared_ptr<Position> GateDatabase::GetPosition(const std::string security_name) const
{
	std::lock_guard<std::mutex> locker(positions_lock_);
	auto it = positions_.find(security_name);
	assert(it != positions_.end() && "You should check existence of the object before trying to get it");
	return it->second;
}

std::shared_ptr<Position> GateDatabase::CreatePosition(const std::shared_ptr<Security>& sec)
{
	std::lock_guard<std::mutex> locker(positions_lock_);
	auto& ptr = positions_.insert(std::make_pair(sec->isin(), std::make_shared<Position>(sec))).first->second;
	ptr->SetGlobalId(GetGlobalId());
	entities_[ptr->global_id()] = ptr;
	return ptr;
}

std::shared_ptr<Entity> GateDatabase::GetEntityByGlobalId(const int id) const
{
	std::lock_guard<std::mutex> locker(entities_lock_);
	auto it = entities_.find(id);
	assert(it != entities_.end() && "You should check existence of the object before trying to get it");
	return it->second;
}

bool GateDatabase::IsPositionExist(const std::string security_name)
{
	std::lock_guard<std::mutex> locker(positions_lock_);
	return (positions_.find(security_name) != positions_.end());
}