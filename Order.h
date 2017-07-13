#pragma once
#include "stdafx.h"
#include "Security.h"
#include "DateTime.h"
#include <memory>
#include "Entity.h"


enum OrderState { kNone = 0, kActive = 1, kDone = 2, kFailed = 3 };
enum OrderType { kPutInQueue = 1, kFillOrKill = 3, kCancelBalance = 2 };
enum TradeDirection {kBuy = 1, kSell = 2};

class Order: public Entity
{
public:
	Order(const std::shared_ptr<Security>& sec, const unsigned int userId);
	Order(const Order& src, const unsigned int userId);
	Order(const Order&) = delete;
	Order& operator=(const Order&) = delete;
	virtual ~Order() = default;

	virtual void Fill(const Order& src);
	
	virtual unsigned int user_id() const; //user sets that id 
	virtual unsigned long long id() const; //exchange sets and gives that id 
	virtual const std::shared_ptr<Security>& security() const;
	virtual TradeDirection direction() const;
	virtual double price() const;
	virtual int volume() const;
	virtual int balance() const;
	virtual OrderType type() const;
	virtual DateTime time() const;
	virtual OrderState state() const;
	virtual int code() const; // return code from exchange
	
	virtual void SetCode(const int code);
	virtual void SetId(const unsigned long long id);
	virtual void SetDirection(const TradeDirection direction);
	virtual void SetPrice(const double price);
	virtual void SetVolume(const int volume);
	virtual void SetBalance(const int balance);
	virtual void SetType(const OrderType type);
	virtual void SetTime(const DateTime& time);
	virtual void SetState(const OrderState state);
private:
	unsigned int user_id_{0};
	unsigned long long id_{0};

	std::shared_ptr<Security> security_;
	TradeDirection direction_{TradeDirection::kBuy};

	double price_{0.0};
	int volume_{0};
	std::atomic<int> balance_{0};
	DateTime time_;
	int code_{0};
	std::atomic<OrderState> state_{OrderState::kNone};
	OrderType type_{OrderType::kPutInQueue};
	
};