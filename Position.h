#pragma once
#include "stdafx.h"
#include <string>
#include <memory>
#include "Security.h"
#include "Entity.h"

class Position:public Entity
{
public:	

	Position(const std::shared_ptr<Security>& sec);
	Position(const std::shared_ptr<Security>& sec, const std::string& client);

	Position(const Position&) = delete;
	Position& operator=(const Position&) = delete;
	virtual ~Position() = default;

	virtual const std::shared_ptr<Security>& security() const;
	virtual double variation_margin() const;
	virtual double price() const;
	virtual int current_amount() const;
	virtual int sells_amount() const;
	virtual int buys_amount() const;
	virtual int open_amount() const;
	virtual std::string const& client_code() const;
	virtual void Fill(const int open, const int buys, const int sells, const int pos, const double price);
	virtual void Fill(const Position& ps);
	virtual void SetVariationMargin(double vm);
private:
	
	std::string client_code_{ "none" }; // account identifier
	int open_amount_{0}; // amount on session's open
	int buys_amount_{0}; // total was bought
	int sell_amount_{0}; // total was sold
	int current_amount_{0}; // current position amount

	double price_{0.0}; // some price of position, not real
	double VM_{0.0}; // PnL from beggining of the session
	const std::shared_ptr<Security> security_;

};