#pragma once
#include "stdafx.h"
#include <string>
#include <atomic>
#include "Entity.h"

enum class SecurityType { kFutures, kOption, kSpread, kEquity, kCurrency };

class Security:public Entity
{
public:

	Security();
	Security(const std::string isin);

	Security(const Security&) = delete;
	Security& operator=(const Security&) = delete;	
	virtual ~Security() = default;

	virtual double margin_buy() const;
	virtual double margin_sell() const;
	virtual int isin_id() const;
	virtual int base_isin_id() const;	//isin of base security 
	virtual double max_price() const;
	virtual double min_price() const;
	virtual double last_price() const;
	virtual int session_id() const;
	virtual bool is_trading_state()const;
	virtual SecurityType type() const;
	virtual std::string const& isin() const;
	
	virtual double ask() const;
	virtual double bid() const;
	virtual int ask_volume() const;
	virtual int bid_volume() const;
	virtual int sess_id() const;

	virtual double GetBidRub() const;
	virtual double GetAskRub() const;
	virtual double ConvertToRubPrice(const double price) const;
	virtual void ConvertPriceToStr(const double price, char* result);
	
	virtual void SetAsk(const double price, const int volume);
	virtual void SetBid(const double price, const int volume);

	virtual void SetSessId(const int sess_id);
	virtual double step_size() const;
	virtual double step_price() const;
	virtual int GetDecimals() const;
	virtual void SetDecimals(const int decimals);
	virtual void SetIsinId(const int isinId);
	virtual void SetBaseIsinId(const int isinId);
	virtual void SetMaxPrice(const double maxPrice);
	virtual void SetMinPrice(const double minPrice);
	virtual void SetLastPrice(const double lastPrice);
	virtual void SetSessionId(const int sessionId);
	virtual void SetStepSize(const double stepSize);
	virtual void SetStepPrice(const double stepPrice);
	virtual void SetTradingState(const bool state);
	virtual void SetType(const SecurityType type);
	virtual void SetMarginBuy(const double margin);
	virtual void SetMarginSell(const double margin);
private:

	
	virtual void Init(const std::string& isin);
	int decimals_;
	int isin_id_;
	int base_isin_id_;
	std::string isin_;
	double max_price_;
	double min_price_;
	double last_price_;
	int sess_id_;
	double step_size_;
	double step_price_;
	bool is_trading_state_;
	SecurityType type_;	
	double margin_buy_;
	double margin_sell_;

	std::atomic<double> ask_;
	std::atomic<int> ask_vol_;

	std::atomic<double> bid_;
	std::atomic<int> bid_vol_;	
};

