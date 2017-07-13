#pragma once
#include "stdafx.h"
#include "Order.h"
#include "cgate.h"
#include "Utils.h"
#include "PlazaMessages.h"
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include "DataStream.h"

enum class TransactionType { kNone = 0, kAddOrder = 1, kCancelOrder = 2, kMoveOrder = 3 };

/*
	stores action with order	
*/
struct Transaction
{
	TransactionType type;
	Order* order_main;
	Order* order_second;

	Transaction()
	{
		type = TransactionType::kNone;
		order_main = nullptr; 
		order_second = nullptr;
	}

	Transaction(TransactionType type_, Order* order, Order *ord2 = nullptr)
	{
		type = type_; 
		order_main = order; 
		order_second = ord2;
	}
};


/*
	posts orders and gets results
	It uses own connection and a pair of publisher/listener.
*/
class TransactionManager
{

public:

	TransactionManager(void *_plaza_connector);
	~TransactionManager();

	TransactionManager(const TransactionManager&) = delete;
	TransactionManager& operator=(const TransactionManager&) = delete;

	void StartPublisher(const std::string& conn_str,const std::string& broker_code, const std::string& client_code);
	void Stop();
	
	void AddOrder(Order * order);
	void CancelOrder(Order *);
	void MoveOrder(Order* old_order, Order* new_order);

	static CG_RESULT CallbackWrapper(cg_conn_t* conn, cg_listener_t* listener, struct cg_msg_t* msg, void* data);

	cg_publisher_t* publisher() const
	{
		return publisher_;
	}

private:

	cg_publisher_t *publisher_;
	cg_listener_t *listener_;
	cg_conn_t *connection_;

	struct cg_msg_t * PrepareFutAddOrder(Order* order); //convert Order (by usual futures or options) to cgate's struct
	struct cg_msg_t * PrepareMultilegOrder(Order* order);//convert Order (by calendar spreads) to cgate's struct

	void PostNewOrder(Order* order); 
	void PostCancelOrder(Order* order);
	void PostMoveOrder(Order* old_order, Order* new_order);
	
	std::thread transaction_thread_; //thread for handling the connection, publisher, and listener
	std::thread publisher_thread_; //thread for processing transaction's queue

	DataStreamState state_;

	std::mutex              queuelock_;
	std::condition_variable queuecheck_;
	std::queue<Transaction> transactions_;
	
	char *broker_code_;
	char *client_code_;
	void *plaza_connector_;

	void TransactionProcess();
	void PublisherProcess();
	std::atomic<bool> exit_;	

};