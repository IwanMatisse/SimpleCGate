#pragma once
#include "../stdafx.h"


namespace simple_cgate
{
	enum data_type { kSecurity = 1, kPosition = 2, kTrade = 3, kOrder = 4, kMoney = 5, kStrategy = 6, kServer = 7, kPositionClear = 8, kRequest = 9 };
	enum request_type { kMessage = 101, kConnect = 102, kDisconnect = 103, kRunStrategy = 104, kStopStrategy = 105 };

#pragma pack(push, 4)
	struct server_data
	{
		int type = data_type::kServer;
		int all_messages = 0;
	};

	struct security_data
	{
		int type = data_type::kSecurity;		
		int id = 0;
		double bid = 0.0;
		int bid_vol = 0;
		double ask = 0.0;
		int ask_vol = 0;
		double last_price = 0.0;
		char symbol[26];
	};

	struct client_request
	{
		int type = data_type::kRequest;
		int req_type = request_type::kMessage;
		int value = 0;
		int id = 0;
	};
	struct position_clear
	{
		int type = data_type::kPositionClear;
	};

	struct position_data
	{
		int type = data_type::kPosition;
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
		int type = data_type::kTrade;
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
		int type = data_type::kOrder;
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
		int type = data_type::kMoney;
		double coeffGo = 0.0;
		double all = 0.0;
		double free = 0.0;
		double blocked = 0.0;
		double fee = 0.0;
	};

	struct strategy_data
	{
		int type = data_type::kStrategy;
		double price = 0.0;
		int volume = 0;
		char state = 0;
		bool enabled = false;
	};
#pragma pack(pop)
} //namespace simple_cgate

//