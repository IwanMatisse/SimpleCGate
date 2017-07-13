#include "stdafx.h"
#include "Position.h"

Position::Position(const std::shared_ptr<Security>& sec):
	security_(sec)
{
	
}

Position::Position(const std::shared_ptr<Security>& sec, const std::string& client) :
	client_code_(client), 
	security_(sec)
{
}

const std::shared_ptr<Security>& Position::security() const
{
	return security_;
}

double Position::variation_margin() const
{
	return VM_;
}

double Position::price() const
{
	return price_;
}

int Position::current_amount() const 
{
	return current_amount_;
}

int Position::sells_amount() const
{
	return sell_amount_;
}

int Position::buys_amount() const
{
	return buys_amount_;
}

int Position::open_amount() const
{
	return open_amount_;
}

std::string const& Position::client_code() const
{
	return client_code_;
}


void Position::Fill(const int open, const int buys, const int sells, const int pos, const double price) 
{	
	open_amount_ = open;
	buys_amount_ = buys;
	sell_amount_ = sells;
	current_amount_ = pos;
	price_ = price;
}

void Position::Fill(const Position& ps)
{	
	open_amount_ = ps.open_amount();
	buys_amount_ = ps.buys_amount();
	sell_amount_ = ps.sells_amount();
	current_amount_ = ps.current_amount();
	price_ = ps.price();
	client_code_ = ps.client_code();
	SetVariationMargin(ps.VM_);
}

void Position::SetVariationMargin(const double vm)
{
	VM_ = vm;
}

