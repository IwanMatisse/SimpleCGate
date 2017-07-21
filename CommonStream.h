#pragma once
#include "stdafx.h"
#include "cgate.h"
#include "DataStream.h"
#include <list>
#include <functional>
#include <array>

/* Provides the best bid and ask by each security
	Source is FORTS_FUTCOMMON_REPL
*/
namespace simple_cgate
{
	class CommonStream : public DataStream
	{
	public:

		static CG_RESULT FutCommonWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data);
		//static CG_RESULT OptCommonWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data);

		CommonStream(void *pl);
		~CommonStream() {}
		void InitPlazaStreams();

		CommonStream(const CommonStream&) = delete;
		CommonStream& operator=(const CommonStream&) = delete;

	};
} //namespace simple_cgate