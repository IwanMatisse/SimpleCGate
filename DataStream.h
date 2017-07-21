#pragma once

#include "stdafx.h"
#include "cgate.h"
#include <vector>
#include <memory>
#include <atomic>
#include <ctime>
#include <thread>
#include "Utils.h"

namespace simple_cgate
{
	enum class DataStreamState { kNone, kError, kOpening, kClosed, kActive };
	enum class ListenerState { kNone, kClose, kOpen, kOnline };

	//wrapper for plaza's listener
	struct PlazaStream
	{
		std::string name;
		bool is_active{ false };
		int life_num;
		bool wait_for{ false };//use for waiting other stream go to online
		std::string replica_state;
		ListenerState state{ ListenerState::kNone };
		cg_listener_t* listener{ nullptr };

		PlazaStream(const std::string& name_)
		{
			name = name_;
		}
	};

	/*
	Implements routines for work with cgate's connection and listeners.
	CGate's connection is created here.
	The listeners are created in the derived classes.

	*/
	class DataStream
	{
	public:

		DataStream(void* connector, const char key, const std::string& mode);
		virtual ~DataStream();

		DataStream(const DataStream&) = delete;
		DataStream& operator=(const DataStream&) = delete;

		virtual void InitPlazaStreams();
		virtual const std::string& name() const;

		void Start(const std::string& conn_str);
		void Stop();

		void Process(); //the main loop to manage states of connections and listeners.

		DataStreamState state() const;
		std::vector<PlazaStream> const& GetPlazaStreams() const;

		virtual void SetWaitForFlag(const bool flag, const int listener_num);//waited listener will not be open until others listeners not online
	protected:

		std::vector<PlazaStream> plaza_streams_;
		cg_conn_t* connection_;
		std::string name_{ "base" };
		void* plaza_;
		std::thread work_thread_;
	private:
		std::atomic<bool> exit_;
		char key_;

		std::string mode_;
		DataStreamState state_;

	};
} //namespace simple_cgate