#pragma once
#include "../stdafx.h"

enum data_type {Security=1, Position=2, Trade=3, Order=4, Money=5, Strategy=6, Server=7, PositionClear=8, Request=9};
enum request_type {Message=1, Connect=2, Disconnect=3, RunStrategy=4, StopStrategy=5};

struct server_data
{
	char type = data_type::Server;
	int all_messages=0;
};

struct security_data
{
	char type = data_type::Security;
	char symbol[26];
	int id = 0;
	double bid = 0.0;
	int bid_vol = 0;
	double ask = 0.0;
	int ask_vol = 0;
	double last_price =0.0;
};

struct client_request
{
	char type = data_type::Request;
	char req_type = request_type::Message;
	int value = 0;
	int id = 0;
};
struct position_clear
{
	char type = data_type::PositionClear;
};

struct position_data
{
	char type = data_type::Position;
	int symbol_id = 0;	
	int open = 0;	
	int buy = 0;
	int sell = 0;
	int current = 0;
	double price = 0.0;
	double vm = 0.0;
};

struct trade_data
{
	char type = data_type::Trade;
	int symbol_id = 0;
	int volume = 0;
	double price = 0.0;
	char direction = 0;
	int id = 0;
	
	//time
};

struct order_data
{
	char type = data_type::Order;
	int symbol_id = 0;
	int volume = 0;
	double price = 0.0;
	char direction = 0;
	int id = 0;
	int state = 0;
	int balance = 0;
	//time
};

struct money_data
{
	char type = data_type::Money;
	double coeffGo = 0.0;
	double all = 0.0;
	double free = 0.0;
	double blocked = 0.0;
	double fee = 0.0;
};

struct strategy_data
{
	char type = data_type::Strategy;
	double price = 0.0;
	int volume = 0;
	char state = 0;
	bool enabled = false;
};

/*

template<typename T> class DataWrapper
{
public:
	T* GetDataPointer() { return &T; }
	int GetSize() { return sizeof(T); }
private:
	T data;
};

class Factory
{
public:
	DataWrapper<security_data> CreateSecurityData(const int global_id);
	DataWrapper<position_data> CreatePositionData(const int global_id);
	DataWrapper<trade_data> CreateTradeData(const int global_id)
	{
		DataWrapper<trade_data> res;
		res
	}
private:
	friend class DataWrapper<security_data>;
	friend class DataWrapper<position_data>;
	friend class DataWrapper<trade_data>;
};*/