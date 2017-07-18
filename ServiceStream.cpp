
#include "stdafx.h"
#include "ServiceStream.h"
#include <thread>
#include "PlazaConnector.h"

namespace simple_cgate
{
	void ServiceStream::InitPlazaStreams()
	{

		auto part = PlazaStream("PART");
		if (CG_ERR_OK != cg_lsn_new(connection_, "p2repl://FORTS_PART_REPL;scheme=|FILE|INI/part.ini|PART",
			&PartWrapper, plaza_, &(part.listener)))
		{
			printf("PlazaConnector: ERROR Cannot create listener for PART_STREAM. \n");
			return;
		}
		part.is_active = true;
		plaza_streams_.push_back(std::move(part));

		auto pos = PlazaStream("POSITIONS");
		if (CG_ERR_OK != cg_lsn_new(connection_, "p2repl://FORTS_POS_REPL;scheme=|FILE|INI/pos.ini|POS",
			&PositionWrapper, plaza_, &(pos.listener)))
		{
			printf("PlazaConnector: ERROR Cannot create listener for POSITIONS_STREAM. \n");
			return;
		}
		pos.wait_for = true;
		pos.is_active = true;
		plaza_streams_.push_back(std::move(pos));

		auto vm = PlazaStream("VM");
		if (CG_ERR_OK != cg_lsn_new(connection_, "p2repl://FORTS_VM_REPL;scheme=|FILE|INI/vm.ini|VM",
			&VMWrapper, plaza_, &(vm.listener)))
		{
			printf("PlazaGate: ERROR Cannot create listener for VM_STREAM. \n");
			return;
		}
		vm.is_active = true;
		vm.wait_for = true;
		plaza_streams_.push_back(std::move(vm));

		auto futinfo = PlazaStream("FUTINFO");
		if (CG_ERR_OK != cg_lsn_new(connection_, "p2repl://FORTS_FUTINFO_REPL;scheme=|FILE|INI/fut_info.ini|FUTINFO",
			&FutInfoWrapper, plaza_, &futinfo.listener))
		{
			printf("PlazaGate: ERROR Cannot create listener for FutInfo_STREAM. \n");
			return;
		}
		futinfo.is_active = true;
		plaza_streams_.push_back(std::move(futinfo));

		work_thread_ = std::move(std::thread(&ServiceStream::Process, this));

	}

	CG_RESULT ServiceStream::PartWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data)
	{
		PlazaConnector *plaza = static_cast<PlazaConnector*>(data);
		return plaza->ListenerPartStream(conn, listener, msg);
	}

	CG_RESULT ServiceStream::PositionWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data)
	{
		PlazaConnector *plaza = static_cast<PlazaConnector*>(data);
		return plaza->ListenerPositionStream(conn, listener, msg);
	}
	CG_RESULT ServiceStream::VMWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data)
	{
		PlazaConnector *plaza = static_cast<PlazaConnector*>(data);
		return plaza->ListenerVMStream(conn, listener, msg);
	}
	CG_RESULT ServiceStream::FutInfoWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data)
	{
		PlazaConnector *plaza = static_cast<PlazaConnector*>(data);
		return plaza->ListenerFutInfoStream(conn, listener, msg);
	}

	ServiceStream::ServiceStream(void *data) :DataStream(data, 'S', "mode=snapshot+online")
	{
		name_ = "ServiceStream";
	}

}//namespace simple_cgate




