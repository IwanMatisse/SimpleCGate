#pragma once
#include "stdafx.h"
#include "Connector.h"
#include "Position.h"
#include "Trade.h"
#include <map>
#include <list>
#include <vector>
#include <set>
#include "cgate.h"
#include "ServiceStream.h"
#include "OrderStream.h"
#include "CommonStream.h"
#include "EventManager.h"
#include "TransactionManager.h"
#include "Database.h"

#include <mutex>
#include <memory>

namespace simple_cgate
{
	class PlazaConnector :public IConnector, public IEventConsumer
	{
	public:
		PlazaConnector(const std::string& path_to_config, EventManager& event_manager, GateDatabase& database);
		~PlazaConnector();

		PlazaConnector(const PlazaConnector&) = delete;
		PlazaConnector& operator=(const PlazaConnector&) = delete;

		void Connect();
		void Disconnect();

		void EnableAutoConnection();
		void DisableAutoConnection();

		void AddOrder(const std::shared_ptr<Order>& order);
		void MoveOrder(const std::shared_ptr<Order>& old_order, const std::shared_ptr<Order>& new_order);
		void DeleteOrder(const std::shared_ptr<Order>& order);

		bool IsFastStreams() const;
		bool IsConnected() const;

		CG_RESULT ListenerFutInfoStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg);
		CG_RESULT ListenerPartStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg);
		CG_RESULT ListenerVMStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg);
		CG_RESULT ListenerPositionStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg);
		CG_RESULT ListenerFutTradeStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg);
		CG_RESULT ListenerFutCommonStream(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg);
		CG_RESULT PublisherCallback(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg);
	private:

		void AddQuoteEvent(const std::shared_ptr<Security>& sec);
		void AddOrderEvent(const std::shared_ptr<Order>& ord);
		void AddMyTradeEvent(const std::shared_ptr<Trade>& deal);
		void AddMoneyEvent(const std::shared_ptr<Money>& mon);
		void AddPositionEvent(const std::shared_ptr<Position>& pos);
		void AddSecurityEvent(const std::shared_ptr<Security>& sec);

		virtual void EventProcess(const Event& _event) override;

		bool InitConfig(const std::string&);
		GateDatabase& database_;

		std::map<long, std::shared_ptr<Security>> securities_id_;

		bool IsSecuirtyExist(long id) const;
		std::shared_ptr<Security> GetSecurityById(long id);
		void AddSecurity(const std::shared_ptr<Security>& sec, long id);

		int last_sess_id = 0;

		std::mutex my_trades_lock_;
		std::mutex orders_lock_;
		std::mutex securities_lock_;
		std::mutex positions_lock_;

		struct GateConfig
		{
			std::string broker_code;
			std::string client_code;
			std::string ip_address;
			std::string port;
			bool is_fast;
			bool tcp;
		} config_;

		int app_id_;

		unsigned int transaction_id{ 1 };
		std::string connection_string_;

		bool is_connected_;
		ServiceStream service_stream_;
		OrderStream order_stream_;
		CommonStream common_stream_;
		TransactionManager transaction_mgr_;

		bool auto_connection_enabled_;
		void AutoConnectionProcess();
		std::thread auto_connection_thread_;

		EventManager& event_manager_;

		void ListenerStateProcess(const DataStream& stream, cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg);

		double ConvertToDouble(const char* str); // Convert the price from cgate's string format to double

		DateTime ConvertToTime(const cg_time_t& cg_time);
	};
} // namespace simple_cgate
