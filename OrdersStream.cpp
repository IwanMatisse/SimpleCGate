
#include "stdafx.h"
#include "OrderStream.h"
#include <thread>
#include "stdafx.h"
#include "PlazaConnector.h"


void OrderStream::InitPlazaStreams()
{
	auto stream = PlazaStream("FUTTRADE");
	if (CG_ERR_OK != cg_lsn_new(connection_, static_cast<PlazaConnector*>(
		plaza_)->IsFastStreams() ?
		"p2repl://FORTS_FUTTRADE_FASTREPL;scheme=|FILE|INI/fut_user_deal.ini|FutUserDeal" :
		"p2repl://FORTS_FUTTRADE_REPL;scheme=|FILE|INI/fut_user_deal.ini|FutUserDeal",
		&FutTradeWrapper, plaza_, &(stream.listener)))
	{
		printf("PlazaConnector: ERROR Cannot create listener for FUTTRADE_STREAM.\n");
		return;
	}
	stream.is_active = true;
	plaza_streams_.push_back(std::move(stream));

	work_thread_ = std::move(std::thread(&OrderStream::Process, this));

}

CG_RESULT OrderStream::FutTradeWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data)
{
	PlazaConnector *plaza = static_cast<PlazaConnector*>(data);
	return plaza->ListenerFutTradeStream(conn, listener, msg);
}


OrderStream::OrderStream(void *data) :DataStream(data, 'T', "mode=online") 
{
	name_ = "OrderStream";
}





