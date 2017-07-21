
#pragma once
#include "stdafx.h"
#include "cgate.h"
#include "DataStream.h"
#include <list>
#include <functional>
#include <array>


/* Provides user's orders and deals
Source is FORTS_FUTTRADE_REPL
*/
namespace simple_cgate
{
	class OrderStream : public DataStream
	{
	public:

		static CG_RESULT FutTradeWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data);
		//static CG_RESULT OptTradeWrapper(cg_conn_t* conn, cg_listener_t* listener, struct  cg_msg_t* msg, void* data);

		OrderStream(void *pl);
		~OrderStream() {}
		void InitPlazaStreams();
		OrderStream(const OrderStream&) = delete;
		OrderStream& operator=(const OrderStream&) = delete;
	};
} // namespace simple_cgate