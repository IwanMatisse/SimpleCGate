#pragma once
#include "stdafx.h"
#include "Security.h"
#include "Money.h"
#include "Order.h"
#include "Trade.h"

/*base interface for connector*/
class IConnector
{
public:
	IConnector() {}
	virtual ~IConnector() {};
	virtual void Connect() = 0 ;
	virtual void Disconnect()=0;
	virtual void EnableAutoConnection()=0;
	virtual void DisableAutoConnection()=0;
	virtual Money const& MoneyInfo() const { return money_info_; }
	virtual void AddOrder(const std::shared_ptr<Order>& order)=0;
	virtual void MoveOrder(const std::shared_ptr<Order>& oldOrder, const std::shared_ptr<Order>& newOrder)=0;
	virtual void DeleteOrder(const std::shared_ptr<Order>& order)=0;	
	virtual bool IsConnected() const =0 ;
	

protected:
	Money money_info_;
	
};