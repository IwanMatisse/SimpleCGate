#include "stdafx.h"
#include "DataStream.h"



DataStream::DataStream(void* connector, const char key, const std::string& mode)
	: work_thread_{}
{
	plaza_ = connector;
	mode_ = mode;
	key_ = key;
	connection_ = nullptr;
	state_ = DataStreamState::kNone;
	exit_ = true;
}

DataStream::~DataStream()
{
	if (exit_ == false)
	{
		Stop();
	}
}

void DataStream::InitPlazaStreams() 
{

}
const std::string& DataStream::name() const { return name_; }


void DataStream::Start(const std::string& conn_str)
{
	if (exit_ == true)
	{
		exit_ = false;

		if (CG_ERR_OK != cg_conn_new((conn_str + std::string{ key_ }).c_str(), &connection_))
		{
			//LogError("{}: Error's appeared by creating connection. conn_str = {}", GetName(), conn_str + std::string{ Key });
		}
		InitPlazaStreams();
	}
}

void DataStream::Stop()
{
	if (exit_ == false)
	{
		exit_ = true;
		state_ = DataStreamState::kNone;
		work_thread_.join();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		plaza_streams_.clear();
	}
}

void DataStream::Process()
{
	uint32_t state = CG_STATE_CLOSED;
	uint32_t state_lsn = CG_STATE_CLOSED;
	
	while (!exit_)
	{
		try
		{
			if (connection_ == nullptr) break;
			
			cg_conn_getstate(connection_, &state);		

			if (state == CG_STATE_ERROR)
			{
				state_ = DataStreamState::kError;
				printf("PlazaConnector: connection in error state.\n");
				
				if (CG_ERR_OK != cg_conn_close(connection_))
				{
					printf("PlazaConnector: %s connection closing error.\n", name().c_str());
				}
			}
			else
				if (state == CG_STATE_CLOSED)
				{
					state_ = DataStreamState::kClosed;					
					printf("PlazaConnector: %s stream is CLOSED.\n", name().c_str());					
					cg_conn_open(connection_, 0);						
				}
				else
					if (state == CG_STATE_OPENING)
					{
						state_ = DataStreamState::kOpening;
						uint32_t result = cg_conn_process(connection_, 1, 0);

						if (result != CG_ERR_OK && result != CG_ERR_TIMEOUT)
						{
							printf("PlazaConnector: %s conn_process in OPENING state. Connection's state request failed.\n", name().c_str());
						}
					}
					else
						if (state == CG_STATE_ACTIVE)
						{
							state_ = DataStreamState::kActive;
							uint32_t result = cg_conn_process(connection_, 1, 0);
							if (result == CG_ERR_OK) //process all messages until function return TIMEOUT 
								for (int i = 0; ((i < 100) && (cg_conn_process(connection_, 0, 0) == CG_ERR_OK)); i++);

							if (result != CG_ERR_OK && result != CG_ERR_TIMEOUT)
							{
								cg_conn_getstate(connection_, &state);								
								printf("PlazaConnector: %s conn_process in ACTIVE state. Coonnection state request failed.\n", name().c_str());								
							}
							
							for (auto plaza_stream : GetPlazaStreams())
							{
								if (plaza_stream.is_active)
								{
									if (!plaza_stream.wait_for && plaza_stream.listener != nullptr)
									{
										cg_lsn_getstate(plaza_stream.listener, &state_lsn);

										switch (state_lsn)
										{
										case CG_STATE_CLOSED:
										{												
											printf("PlazaConnector: %s listener: %s CLOSED.\n", name().c_str(), plaza_stream.name.c_str());
											cg_lsn_open(plaza_stream.listener, mode_.c_str());
											break;
										}
										case CG_STATE_ERROR:
											printf("PlazaConnector: %s listener: %s IN ERROR. try to close...\n", name().c_str(), plaza_stream.name.c_str());
											cg_lsn_close(plaza_stream.listener);
											break;
										}
									}										
								}
							}				
							
						}
		}
		catch (...) 
		{
			printf("PlazaConnector: Exception thrown in %s main function\n", name().c_str());			
			exit_ = true;
		}
	}
	
	for (auto plaza_stream: GetPlazaStreams())
	{
		if (plaza_stream.is_active)
			if (plaza_stream.listener != nullptr)
			{
				cg_lsn_close(plaza_stream.listener);
				cg_lsn_destroy(plaza_stream.listener);
				plaza_stream.listener = nullptr;
			}
	}	
		
	if (connection_ != nullptr)
	{
		cg_conn_destroy(connection_);
		connection_ = nullptr;
	}
}

void DataStream::SetWaitForFlag(const bool flag, const int listener_num)
{
	if (listener_num > 0 && listener_num < plaza_streams_.size())
	{
		plaza_streams_[listener_num].wait_for = flag;
	}
}

std::vector<PlazaStream> const& DataStream::GetPlazaStreams() const
{
	return plaza_streams_;
}


DataStreamState DataStream::state() const
{ 
	return state_; 
}