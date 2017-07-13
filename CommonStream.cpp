
#include "stdafx.h"
#include "CommonStream.h"
#include <thread>
#include "PlazaConnector.h"


void CommonStream::InitPlazaStreams()
{
	auto common = PlazaStream("FUTCOMMON");
	if (CG_ERR_OK != cg_lsn_new(connection_, "p2repl://FORTS_FUTCOMMON_REPL;scheme=|FILE|INI/fut_common.ini|CustReplScheme",
		&FutCommonWrapper, plaza_, &common.listener))
	{
		printf("PlazaConnector: ERROR Cannot create listener for FUTCOMMON_STREAM.\n");
		return;
	}
	common.is_active = true;
	plaza_streams_.push_back(std::move(common));
	work_thread_ = std::move(std::thread(&OrderStream::Process, this));
}

CG_RESULT CommonStream::FutCommonWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data)
{
	PlazaConnector *plaza = static_cast<PlazaConnector*>(data);
	return plaza->ListenerFutCommonStream(conn, listener, msg);
}

/*CG_RESULT CommonStream::OptCommonWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data)
{
	PlazaConnector *plaza = static_cast<PlazaConnector*>(data);
	return plaza->ListenerOptCommonStream(conn, listener, msg);
}*/


CommonStream::CommonStream(void *data) :DataStream(data, 'C', "mode=online")
{
	name_ = "CommonStream";
}





