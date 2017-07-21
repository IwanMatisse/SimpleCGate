
#pragma once
#include "stdafx.h"
#include "cgate.h"
#include "DataStream.h"
#include <list>
#include <functional>
#include <array>

/* Provides positions, variational margin, funds, limits and instruments definition
Sources: FORTS_PART_REPL, FORTS_POS_REPL, FORTS_VM_REPL, FORTS_FUTINFO_REPL
*/
namespace simple_cgate
{

	class ServiceStream : public DataStream
	{
	public:

		static CG_RESULT PartWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data);
		static CG_RESULT PositionWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data);
		static CG_RESULT VMWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data);
		static CG_RESULT FutInfoWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data);


		ServiceStream(void *connector);
		~ServiceStream() {}
		void InitPlazaStreams();
		ServiceStream(const ServiceStream&) = delete;
		ServiceStream& operator=(const ServiceStream&) = delete;
	};

}// namespace simple_cgate