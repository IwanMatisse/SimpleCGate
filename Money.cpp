#include "stdafx.h"
#include "Money.h"

double Money::coeff_GO() const
{
	return coeffGo_;
}

double Money::all_amount() const
{
	return all_;
}

double Money::free_amount() const
{
	return free_;
}

double Money::blocked_amount() const
{
	return blocked_;
}

double Money::fee() const
{
	return fee_;
}

void Money::SetCoeffGO(const double coeffGo)
{
	coeffGo_ = coeffGo;
}

void Money::SetAll(const double moneyAll)
{
	all_ = moneyAll;
}

void Money::SetFree(const double moneyFree)
{
	free_ = moneyFree;
}

void Money::SetBlocked(const double moneyBlocked)
{
	blocked_ = moneyBlocked;
}

void Money::SetFee(const double fee) 
{
	fee_ = fee;
}