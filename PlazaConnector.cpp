#include "stdafx.h"
#include "PlazaConnector.h"
#include "Utils.h"


void PlazaConnector::AutoConnectionProcess()
{
	while (auto_connection_enabled_)
	{
		auto now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm time;
		localtime_s(&time, &now_c);
		int curtime = time.tm_hour * 100 + time.tm_min;

		if (time.tm_wday != 0 && time.tm_wday != 6 //saturday and sunday are day off
			&& curtime > 945 && curtime < 2350) //sample 945 = 9:45 start connection
		{			
			Connect();
		}
		else
		{			
			Disconnect();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));		
	}
}

void PlazaConnector::AddQuoteEvent(const std::shared_ptr<Security>& sec)
{	
	event_manager_.AddEvent(Event::Types::kQuoteChange, sec->global_id());
}

void PlazaConnector::AddOrderEvent(const std::shared_ptr<Order>& ord)
{
	event_manager_.AddEvent(Event::Types::kOrderChange, ord->global_id());	
}
void PlazaConnector::AddMyTradeEvent(const std::shared_ptr<Trade>& deal)
{
	event_manager_.AddEvent(Event::Types::kMyTradeAdd, deal->global_id());
}

void PlazaConnector::AddMoneyEvent(const std::shared_ptr<Money>& money)
{
	event_manager_.AddEvent(Event::Types::kMoneyChange);
}
void PlazaConnector::AddPositionEvent(const std::shared_ptr<Position>& pos)
{
	event_manager_.AddEvent(Event::Types::kPositionChange, pos->global_id());
}
void PlazaConnector::AddSecurityEvent(const std::shared_ptr<Security>& sec)
{
	event_manager_.AddEvent(Event::Types::kSecurityAdd, sec->global_id());
}

bool PlazaConnector::IsFastStreams()const  
{
	return config_.is_fast; 
}

bool PlazaConnector::InitConfig(const std::string& path)
{	
	auto conf = ReadConfigFile(path);
	if (conf.find("main") == conf.end())
	{
		// "config is corrupt!"	
		return false;
	}
	else
	{
		auto map = conf["main"];
		ReadParameter(map, "BrokerCode", config_.broker_code);
		ReadParameter(map, "ClientCode", config_.client_code);
		ReadParameter(map, "Address", config_.ip_address);
		ReadParameter(map, "Port", config_.port);
		ReadParameter(map, "Fast", config_.is_fast);
		ReadParameter(map, "TCP", config_.tcp);
	}
	std::stringstream st;
	st << (config_.tcp?"p2tcp://" : "p2lrpcq://") << config_.ip_address << ":" << config_.port << ";app_name=SIMPLE" << app_id_;
	connection_string_ =   st.str();
	return true;
}

void PlazaConnector::AddOrder(const std::shared_ptr<Order>& order)
{
	if (order != nullptr)
	{
		transaction_mgr_.AddOrder(order.get());
		event_manager_.AddEvent(Event::Types::kOrderChange, order->global_id());
	}
}
void PlazaConnector::MoveOrder(const std::shared_ptr<Order>& oldOrder, const std::shared_ptr<Order>& newOrder)
{
	if (oldOrder != nullptr && newOrder != nullptr)
	{
		transaction_mgr_.MoveOrder(oldOrder.get(), newOrder.get());
		event_manager_.AddEvent(Event::Types::kOrderChange, oldOrder->global_id());
		event_manager_.AddEvent(Event::Types::kOrderChange, newOrder->global_id());
	}
}
void PlazaConnector::DeleteOrder(const std::shared_ptr<Order>& order)
{
	if (order != nullptr && order->state() == OrderState::kActive)
	{
		transaction_mgr_.CancelOrder(order.get());
		event_manager_.AddEvent(Event::Types::kOrderChange, order->global_id());
	}
}

bool PlazaConnector::IsConnected() const
{
	return is_connected_;
}

void PlazaConnector::EnableAutoConnection()
{
	{
		if (!auto_connection_enabled_)
		{			
			auto_connection_enabled_ = true;
			auto_connection_thread_ = std::move(std::thread(&PlazaConnector::AutoConnectionProcess, this));
		}
	}
}

void PlazaConnector::DisableAutoConnection()
{
	if (auto_connection_enabled_)
	{		
		auto_connection_enabled_ = false;
		auto_connection_thread_.join();
	}
	auto_connection_enabled_ = false;
}

bool PlazaConnector::IsSecuirtyExist(long id) const
{
	return securities_id_.find(id) != securities_id_.end();
}
std::shared_ptr<Security> PlazaConnector::GetSecurityById(long id)
{
	return securities_id_[id];
}

void PlazaConnector::AddSecurity(const std::shared_ptr<Security>& sec, long id)
{
	securities_id_[id] = sec;
}

PlazaConnector::PlazaConnector(const std::string& pathToConfig, EventManager& eventManager, GateDatabase& database)
	:event_manager_{ eventManager },
	database_{database},
	service_stream_(this), 
	order_stream_(this), 
	common_stream_(this), 
	transaction_mgr_(this),
	auto_connection_enabled_{ false }, 
	is_connected_{ false }
{

	srand(static_cast<int>(time(NULL)));
	app_id_ = rand() % 90 + 10;	

	InitConfig(pathToConfig);	
	
	std::string st2{ "ini=netrepl.ini;key=11111111" };

	if (cg_env_open(st2.c_str()) != CG_ERR_OK)
	{
		printf("PlazaConnector: environment intialization error\n");
		throw "Plaza gate intialization error";
	}

	transaction_id = app_id_ * 1000000;	
}

PlazaConnector::~PlazaConnector()
{
	DisableAutoConnection();
	Disconnect();
	if (cg_env_close() != CG_ERR_OK)
	{
		printf("PlazaConnector: error on enviroment's closing\n");
	}	
}

void PlazaConnector::Connect()
{
	if (!is_connected_)
	{		
		is_connected_ = true;
		printf("PlazaConnector: Connecting start...\n");	

		service_stream_.Start(connection_string_);			
		transaction_mgr_.StartPublisher(connection_string_, config_.broker_code, config_.client_code);					
		order_stream_.Start(connection_string_);
		common_stream_.Start(connection_string_);		
		event_manager_.AddEvent(Event::Types::kConnect, 0);
		
	}
}

void PlazaConnector::Disconnect()
{
	if (is_connected_)
	{
		is_connected_ = false;
		printf("PlazaConnector: Disconnect...\n");
		
		transaction_mgr_.Stop();		
		common_stream_.Stop();		
		order_stream_.Stop();		
		service_stream_.Stop();
		event_manager_.AddEvent(Event::Types::kDisconnect, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}


double PlazaConnector::ConvertToDouble(const char* str) // Convert price from cgate's string format to double
{
	int64_t price;
	int8_t scale;
	cg_bcd_get(str, &price, &scale);
	return (double)price / pow(10, scale);
}

DateTime PlazaConnector::ConvertToTime(const cg_time_t& cg_time)
{
	return DateTime(cg_time.year, cg_time.month, cg_time.day, cg_time.hour, cg_time.minute, cg_time.second, cg_time.msec);
}