#pragma once
#include "Strategy.h"
#include "Security.h"
#include "Order.h"
#include "Database.h"

/*
It is just a sample strategy
Trade rules:
Sell when the best ask's volume is more then <signal_volume>
Buy when the best bid's bolume is more then <signal_volume>

*/
namespace simple_cgate
{
	class SampleBidAskStrategy :public Strategy
	{
	public:


		void OnQuotesChange() //trade logic
		{
			if (current_order_ == nullptr)
			{
				if (current_position_ >= 0 && security_->ask_volume() > signal_volume_)
				{
					int vol = work_volume_; //calc volume
					if (current_position_ > 0)
						vol = current_position_;

					//order's initialize
					current_order_ = database_.CreateNewOrder(security_);
					current_order_->SetPrice(security_->ask() - security_->step_size());
					current_order_->SetDirection(kSell);
					current_order_->SetBalance(vol);
					current_order_->SetType(OrderType::kPutInQueue);
					current_order_->SetVolume(vol);
					PostOrder(current_order_);//send order

					orders_[current_order_->user_id()] = current_order_;//save in the local container
					printf("SBA: SELL %d by %f \n", vol, current_order_->price());
				}
				else
					if (current_position_ <= 0 && security_->bid_volume() > signal_volume_)
					{
						int vol = work_volume_;
						if (current_position_ < 0)
							vol = -current_position_;
						current_order_ = database_.CreateNewOrder(security_);
						current_order_->SetPrice(security_->bid() + security_->step_size());
						current_order_->SetDirection(kBuy);
						current_order_->SetBalance(vol);
						current_order_->SetType(OrderType::kPutInQueue);
						current_order_->SetVolume(vol);

						PostOrder(current_order_);
						orders_[current_order_->user_id()] = current_order_;
						printf("SBA: BUY %d by %f \n", vol, current_order_->price());
					}
			}
			else
			{

			}
		}



		void OnOrderChange(const int id)
		{
			if (current_order_ != nullptr && current_order_->global_id() == id) //Is order from that strategy?
			{
				if (current_order_->state() == OrderState::kActive)
				{
					if (current_order_->time().minute() != Now().minute())
					{
						CancelOrder(current_order_);
						current_order_ = nullptr;
						printf("SBA: Order was canceled by time \n");
					}
				}
			}
		}

		void OnNewTrade(const int id)
		{
			auto trade = database_.GetTradeByGlobalId(id);

			if (orders_.find(trade->order()->id()) != orders_.end()) //check if the trade is by an order from this strategy
			{
				if (trade->direction() == TradeDirection::kBuy)
					current_position_ += trade->volume();
				else
					current_position_ -= trade->volume();

				printf("SBA: New trade has arrived; Position is %d \n", current_position_);
			}
		}

		SampleBidAskStrategy(GateDatabase& db) :
			Strategy("BidAskVolume"),
			database_{ db }
		{
		}
		bool IsTradeTime(const DateTime& time)
		{
			return (time.hour() >= 10 && time.hour() <= 23);
		}
		void ResetStatus()
		{
			current_order_ = nullptr;
			current_position_ = 0;
		}
		void OnDisconnect()
		{

		}

		void LoadConfig()
		{
			//In real application, there should be loading strategy's settings from file or db
			symbol_ = "RTS-6.17";
			signal_volume_ = 100;
			work_volume_ = 10;

			if (database_.IsSecurityExist(symbol_))
				security_ = database_.GetSecurity(symbol_);
			else
				security_ = database_.CreateSecurity(symbol_);
			printf("SBA: Configuration loaded \n");
		}
		void SaveConfig()
		{
			//place holder for saving strategy's settings to file or db		
		}

		void OnBackgroundEvent()
		{
			//place holder for some background calculations, actions. For example, making history bars or candles from current quotes		
		}

		void OnNewSecurity(const int id)
		{
		}
		std::vector<std::shared_ptr<Security>> GetUsedSecurity()
		{
			return std::vector<std::shared_ptr<Security>>{security_};
		}

	private:
		std::string symbol_;
		int signal_volume_;
		int work_volume_;

		std::map<unsigned long long, std::shared_ptr<Order>> orders_;
		std::shared_ptr<Security> security_;

		int current_position_{ 0 };
		std::shared_ptr<Order> current_order_{ nullptr };
		GateDatabase& database_;
	};
}//namespace simple_cgate