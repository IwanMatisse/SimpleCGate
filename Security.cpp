#include "stdafx.h"
#include "Security.h"

void Security::Init(const std::string& isin)
{
	SetDecimals(0);
	SetIsinId(0);
	SetBaseIsinId(0);
	isin_ = isin;
	SetMaxPrice(0.0);
	SetMinPrice(0.0);
	SetLastPrice(0.0);
	SetSessionId(0);
	SetStepPrice(0.0);
	SetStepSize(0.0);
	SetTradingState(false);
	SetType(SecurityType::kFutures);
	SetMarginBuy(0.0);
	SetMarginSell(0.0);
	SetAsk(0.0, 0);
	SetBid(0.0, 0);
}

Security::Security()
{
	Init("");
}
Security::Security(const std::string isin)
{
	Init(isin);
}

void Security::SetAsk(const double price, const int volume)
{
	ask_ = price;
	ask_vol_ = volume;
}
void Security::SetBid(const double price, const int volume)
{
	bid_ = price;
	bid_vol_ = volume;
}

int Security::GetDecimals() const
{
	return decimals_;
}

void Security::SetDecimals(const int decimals)
{
	decimals_ = decimals;
}

int Security::isin_id() const
{
	return isin_id_;
}

void Security::SetIsinId(const int isinId)
{
	isin_id_ = isinId;
}

const std::string& Security::isin() const
{
	return isin_;
}

int Security::sess_id() const
{
	return sess_id_;
}

void Security::SetSessId(const int sess_id) 
{
	sess_id_ = sess_id;
}

int Security::base_isin_id() const
{
	return base_isin_id_;
}

void Security::SetBaseIsinId(const int isinId)
{
	base_isin_id_ = isinId;
}

double Security::max_price() const
{
	return max_price_;
}

void Security::SetMaxPrice(const double maxPrice)
{
	max_price_ = maxPrice;
}

double Security::min_price() const
{
	return min_price_;
}

void Security::SetMinPrice(const double minPrice)
{
	min_price_ = minPrice;
}

double Security::last_price() const
{
	return last_price_;
}

void Security::SetLastPrice(const double lastPrice)
{
	last_price_ = lastPrice;
}

int Security::session_id() const
{
	return sess_id_;
}

void Security::SetSessionId(const int sessionId)
{
	sess_id_ = sessionId;
}

double Security::step_size() const
{
	return step_size_;
}

void Security::SetStepSize(const double stepSize)
{
	step_size_ = stepSize;
}

double Security::step_price() const
{
	return step_price_;
}

void Security::SetStepPrice(const double stepPrice)
{
	step_price_ = stepPrice;
}

bool Security::is_trading_state()const
{
	return is_trading_state_;
}

void Security::SetTradingState(const bool state)
{
	is_trading_state_ = state;
}

SecurityType Security::type() const
{
	return type_;
}

void Security::SetType(const SecurityType type)
{
	type_ = type;
}

double Security::margin_buy() const
{
	return margin_buy_;
}

void Security::SetMarginBuy(const double margin)
{
	margin_buy_ = margin;
}

double Security::margin_sell() const
{
	return margin_sell_;
}

void Security::SetMarginSell(const double margin)
{
	margin_sell_ = margin;
}

double Security::ask() const
{
	return ask_;
}

double Security::bid() const
{
	return bid_;
}

int Security::ask_volume() const
{
	return ask_vol_;
}

int Security::bid_volume() const
{
	return bid_vol_;
}

double Security::GetBidRub() const
{
	return bid()*step_price() / step_size();
}
double Security::GetAskRub() const
{
	return ask()*step_price() / step_size();
}

double Security::ConvertToRubPrice(const double price) const
{
	return price*step_price() / step_size();
}

void Security::ConvertPriceToStr(const double price, char* result)
{
	constexpr int ResultLength = 18;
	constexpr int AdditionalSings = 7; // multiply on 1 000 000

	if (price == 0.0)
	{
		strcpy_s(result, ResultLength, "0.00000");
		return;
	}
	double stepSize = step_size();
	if (stepSize != 0.0)
	{
		bool minus = (price < 0 ? true : false);	
		long long longPrice = static_cast<long long>(round(abs(price) / stepSize)*stepSize * 1000000 + 1);
		if (longPrice == 1)
		{
			strcpy_s(result, ResultLength, "0.00000");
			return;
		}
		char cprice[ResultLength + AdditionalSings];
		_i64toa_s(longPrice, cprice, ResultLength + AdditionalSings, 10);
		int len = 0;
		for (int i = 0; i < ResultLength + AdditionalSings; i++)
		{
			if (cprice[i] == 0)
			{
				len = i + 1;
				break;
			}
		}

		if (minus)
		{
			for (int i = len - 1; i > 0; i--)
				cprice[i] = cprice[i - 1];
			cprice[0] = '-';
			len++;
		}

		for (int i = 0; i < 5; i++)
			cprice[len - 2 - i] = cprice[len - 2 - i - 1];
		cprice[len - AdditionalSings] = '.';
		cprice[len - 1] = 0;
		memcpy(result, cprice, len);	

	}
}