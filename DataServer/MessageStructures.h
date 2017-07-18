#pragma once
#include "../stdafx.h"

enum data_type {kSecurity=1, kPosition=2, kTrade=3, kOrder=4, kMoney=5, kStrategy=6, kServer=7, kPositionClear=8, kRequest=9};
enum request_type {kMessage=1, kConnect=2, kDisconnect=3, kRunStrategy=4, kStopStrategy=5};

struct server_data
{
	char type = data_type::kServer;
	int all_messages=0;
};

struct security_data
{
	char type = data_type::kSecurity;
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
	char type = data_type::kRequest;
	char req_type = request_type::kMessage;
	int value = 0;
	int id = 0;
};
struct position_clear
{
	char type = data_type::kPositionClear;
};

struct position_data
{
	char type = data_type::kPosition;
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
	char type = data_type::kTrade;
	int symbol_id = 0;
	int volume = 0;
	double price = 0.0;
	char direction = 0;
	int id = 0;
	date_struct date;
	time_struct time;
};

struct order_data
{
	char type = data_type::kOrder;
	int symbol_id = 0;
	int volume = 0;
	double price = 0.0;
	char direction = 0;
	int id = 0;
	int state = 0;
	int balance = 0;
	date_struct date;
	time_struct time;
};

struct money_data
{
	char type = data_type::kMoney;
	double coeffGo = 0.0;
	double all = 0.0;
	double free = 0.0;
	double blocked = 0.0;
	double fee = 0.0;
};

struct strategy_data
{
	char type = data_type::kStrategy;
	double price = 0.0;
	int volume = 0;
	char state = 0;
	bool enabled = false;
};
