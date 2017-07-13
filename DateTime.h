#pragma once
#include "stdafx.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>

class DateTime
{
public:
	DateTime() {}

	DateTime(int y, int m, int d, int H, int M, int S, int MS)
	{
		year_ = y;
		month_ = m;
		day_ = d;
		hour_ = H;
		minute_ = M;
		second_ = S;
		msecond_ = MS;
	}

	std::string GetDate()
	{
		std::stringstream ss;
		ss.precision(2);
		ss <<day_ << "-" << month_ << "-" << year_;
		return ss.str();
	}

	std::string GetTime()
	{
		std::stringstream ss;
		ss.precision(2);
		ss << hour_ << ":" << minute_ << ":" << second_ << "." << msecond_;
		return ss.str();
	}

	std::string GetDateTime()
	{
		std::stringstream ss;
		ss.precision(2);
		ss << day_ << "-" << month_ << "-" << year_ <<" "<< hour_ << ":" << minute_ << ":" << second_ << "." << msecond_;
		return ss.str();
	}

	int year()const
	{
		return year_;
	}
	int month()const
	{
		return month_;
	}
	int day()const
	{
		return day_;
	}
	int hour()const
	{
		return hour_;
	}
	int minute()const
	{
		return minute_;
	}
	int second()const
	{
		return second_;
	}
	int millisecond()const
	{
		return msecond_;
	}
private:
	int year_ {0};
	int month_{0};
	int day_{0};
	int hour_{0};
	int minute_{0};
	int second_{0};
	int msecond_{0};
};