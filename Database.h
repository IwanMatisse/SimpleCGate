#pragma once
#include "stdafx.h"
#include "Money.h"
#include "Trade.h"
#include "Position.h"
#include "Strategy.h"
#include <list>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>

/*
The GateDatabase contains dictionaries of instruments, positions, orders and trades and provides thread-safe access to them. 
Each entity has a unique identifier (global_id) and can have additional identifiers provided by the MOEX. 
For example, ISIN and isin_id for Futures or order_id for Orders. 
The GateDatabase stores data in several data structures to provide fast access by any identifier.
*/
namespace simple_cgate
{

	class GateDatabase
	{

	public:

		GateDatabase();
		virtual ~GateDatabase() = default;

		virtual std::shared_ptr<Money> money_info();
		virtual std::shared_ptr<Entity> GetEntityByGlobalId(const int id) const;

		virtual std::shared_ptr<Security> GetSecurityByGlobalId(const int id) const;
		virtual std::shared_ptr<Security> GetSecurity(const std::string name) const;
		virtual std::shared_ptr<Security> CreateSecurity(const std::string name);
		virtual bool IsSecurityExist(const std::string name);

		virtual std::shared_ptr<Order> GetOrder(const unsigned int user_id) const;
		virtual std::shared_ptr<Order> GetOrderByGlobalId(const int id) const;
		virtual std::shared_ptr<Order> CreateOrder(const unsigned int user_id, const std::shared_ptr<Security>& sec);
		virtual std::shared_ptr<Order> CreateNewOrder(const std::shared_ptr<Security>& sec);
		virtual bool IsOrderExist(const unsigned int user_id);

		virtual std::shared_ptr<Trade> GetTradeByGlobalId(const int id) const;
		virtual std::shared_ptr<Trade> GetTrade(const unsigned long long id) const;
		virtual std::shared_ptr<Trade> CreateTrade(const unsigned long long id, const std::shared_ptr<Security>& sec, const std::shared_ptr<Order>& ord);
		virtual bool IsTradeExist(const unsigned long long id);

		virtual std::shared_ptr<Strategy> AddStrategy(const std::shared_ptr<Strategy>& ptr);
		virtual std::shared_ptr<Strategy> GetStrategyByGlobalId(const int id) const;

		virtual void ClearPosition();
		virtual std::shared_ptr<Position> GetPositionByGlobalId(const int id) const;
		virtual std::shared_ptr<Position> GetPosition(const std::string security_name) const;
		virtual std::shared_ptr<Position> CreatePosition(const std::shared_ptr<Security>& sec);
		virtual bool IsPositionExist(const std::string security_name);

	private:
		unsigned int transaction_id{ 1 };
		std::atomic<int> global_id_{ 1 };
		virtual int GetGlobalId()
		{
			return global_id_.fetch_add(1);
		}

		std::map<int, std::shared_ptr<Entity>> entities_;
		std::map<std::string, std::shared_ptr<Security>> securities_;
		std::map<unsigned int, std::shared_ptr<Order>> orders_;
		std::map<unsigned long long, std::shared_ptr<Trade>> my_trades_;
		std::map<std::string, std::shared_ptr<Position>> positions_; //by ticker's name	
		std::shared_ptr<Money> money_info_;

		mutable std::mutex orders_lock_;
		mutable std::mutex securities_lock_;
		mutable std::mutex positions_lock_;
		mutable std::mutex my_trades_lock_;
		mutable std::mutex entities_lock_;

	};

} // namespace simple_cgate
