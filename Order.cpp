#include "stdafx.h"
#include "Order.h"

namespace simple_cgate
{
	Order::Order(const std::shared_ptr<Security>& sec, const unsigned int userId) :
		security_(sec)
	{
		user_id_ = userId;		
	}


	Order::Order(const Order& src, const unsigned int userId) : security_(src.security())
	{
		Fill(src);
		user_id_ = userId;
		SetId(0);
	}

	void Order::Fill(const Order& src)
	{
		SetId(src.id());
		SetDirection(src.direction());
		SetPrice(src.price());
		SetVolume(src.volume());
		SetBalance(src.balance());
		SetType(src.type());
		SetTime(src.time());
		SetState(OrderState::kNone);
	}

	unsigned int Order::user_id() const
	{
		return user_id_;
	}

	unsigned long long Order::id() const
	{
		return id_;
	}
	void Order::SetId(const unsigned long long id)
	{
		id_ = id;
	}
	const std::shared_ptr<Security>& Order::security() const
	{
		return security_;
	}

	TradeDirection Order::direction() const
	{
		return direction_;
	}
	void Order::SetDirection(const TradeDirection direction)
	{
		direction_ = direction;
	}
	double Order::price() const
	{
		return price_;
	}
	void Order::SetPrice(const double price)
	{
		price_ = price;
	}
	int Order::volume() const
	{
		return volume_;
	}
	void Order::SetVolume(const int volume)
	{
		volume_ = volume;
	}
	int Order::balance() const
	{
		return balance_;
	}
	void Order::SetBalance(const int balance)
	{
		balance_ = balance;
	}
	OrderType Order::type() const
	{
		return type_;
	}
	void Order::SetType(const OrderType type)
	{
		type_ = type;
	}
	DateTime Order::time() const
	{
		return time_;
	}
	void Order::SetTime(const DateTime& time)
	{
		time_ = time;
	}

	OrderState Order::state() const
	{
		return state_;
	}

	void Order::SetState(const OrderState state)
	{
		state_ = state;
	}


	int Order::code() const
	{
		return code_;
	}

	void Order::SetCode(const int code)
	{
		code_ = code;
	}

}//namespace simple_cgate