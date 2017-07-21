#include "stdafx.h"
#include "StrategyManager.h"

namespace simple_cgate
{
	void StrategyManager::EventProcess(const Event& event_)
	{
		switch (event_.type)
		{
		case Event::Types::kSecurityAdd:
		{
			std::lock_guard<std::mutex> locker(algo_lock_);
			for (auto strategy : strategies_)
			{
				strategy->OnNewSecurity(event_.global_id);
			}
			break;
		}
		case Event::Types::kMyTradeAdd:
		{
			std::lock_guard<std::mutex> locker(algo_lock_);
			for (auto strategy : strategies_)
			{
				strategy->OnNewTrade(event_.global_id);
				event_manager_.AddEvent(Event::Types::kStrategyChange, strategy->global_id());
			}
			break;
		}
		case Event::Types::kQuoteChange:
		{
			std::lock_guard<std::mutex> locker(algo_lock_);
			if (strategies_map_.find(event_.global_id) != strategies_map_.end())
			{
				for (auto strategy : strategies_map_[event_.global_id])
				{
					strategy->OnQuotesChange();
					event_manager_.AddEvent(Event::Types::kStrategyChange, strategy->global_id());
				}
			}

			break;
		}

		case Event::Types::kOrderChange:
		{
			std::lock_guard<std::mutex> locker(algo_lock_);
			for (auto strategy : strategies_)
			{
				strategy->OnOrderChange(event_.global_id);
				event_manager_.AddEvent(Event::Types::kStrategyChange, strategy->global_id());
			}
			break;
		}
		case Event::Types::kDisconnect:
		{
			OnDisconnect();
			break;
		}
		case Event::Types::kConnect:
		{
			is_connected_ = true;
			break;
		}
		}

	}

	void StrategyManager::BackgroundProcess()
	{
		while (!exit_flag_)
		{
			if (is_connected_)
			{
				try
				{
					std::lock_guard<std::mutex> locker(algo_lock_);
					for (auto strategy : strategies_)
					{
						strategy->OnBackgroundEvent();//some background computing in algos
					}

				}
				catch (...)
				{
					printf("Algomanager: Exception on background process \n");
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		printf("Algomanager: background process was terminated \n");
	}



	void StrategyManager::AddStrategy(const std::shared_ptr<Strategy>& strategy, std::vector<std::shared_ptr<Security>> securities)
	{
		auto str = database_.AddStrategy(strategy);

		{
			std::lock_guard<std::mutex> locker(algo_lock_);
			strategies_.push_back(str);

			for (auto security : securities)
			{
				if (strategies_map_.find(security->global_id()) != strategies_map_.end())
				{
					strategies_map_[security->global_id()].push_back(str);
				}
				else
				{
					strategies_map_[security->global_id()] = std::vector<std::shared_ptr<Strategy>>{ str };
				}
			}
		}

		event_manager_.AddEvent(Event::Types::kNewStrategy, str->global_id());
	}


	StrategyManager::StrategyManager(EventManager& eve, GateDatabase& db)
		:event_manager_{ eve },
		database_{ db }
	{
		exit_flag_ = false;
		background_thread_ = std::move(std::thread(&StrategyManager::BackgroundProcess, this));
		event_manager_.AddNewConsumer(this);
	}

	StrategyManager::~StrategyManager()
	{
		if (exit_flag_ == false)
		{
			exit_flag_ = true;

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if (background_thread_.joinable())
				background_thread_.join();
		}
		event_manager_.RemoveConsumer(this);
	}

	void StrategyManager::OnDisconnect()
	{
		is_connected_ = false;
		std::lock_guard<std::mutex> locker(algo_lock_);
		for (auto strategy : strategies_)
		{
			strategy->OnDisconnect();
		}
	}

}//namespace simple_cgate