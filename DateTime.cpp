
#include "stdafx.h"
#include "DateTime.h"
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>

DateTime::DateTime(int y, int m, int d, int H, int M, int S, int MS)
{
	year_ = y;
	month_ = m;
	day_ = d;
	hour_ = H;
	minute_ = M;
	second_ = S;
	msecond_ = MS;
}

date_struct DateTime::GetDateStruct() const
{
	date_struct res;
	res.year = year();
	res.month = month();
	res.day = day();
	return res;
}

time_struct DateTime::GetTimeStruct() const
{
	time_struct res;
	res.HH = hour();
	res.mm = minute();
	res.ss = second();
	res.ms = millisecond();
	return res;
}

std::string DateTime::GetDate()
{
	std::stringstream ss;
	ss.precision(2);
	ss << day_ << "-" << month_ << "-" << year_;
	return ss.str();
}

std::string DateTime::GetTime()
{
	std::stringstream ss;
	ss.precision(2);
	ss << hour_ << ":" << minute_ << ":" << second_ << "." << msecond_;
	return ss.str();
}

std::string DateTime::GetDateTime()
{
	std::stringstream ss;
	ss.precision(2);
	ss << day_ << "-" << month_ << "-" << year_ << " " << hour_ << ":" << minute_ << ":" << second_ << "." << msecond_;
	return ss.str();
}

int DateTime::year()const
{
	return year_;
}
int DateTime::month()const
{
	return month_;
}
int DateTime::day()const
{
	return day_;
}
int DateTime::hour()const
{
	return hour_;
}
int DateTime::minute()const
{
	return minute_;
}
int DateTime::second()const
{
	return second_;
}
int DateTime::millisecond()const
{
	return msecond_;
}
