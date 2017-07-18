#pragma once
#include "stdafx.h"
#include <string>


#pragma pack(push, 4)
struct date_struct
{
	int year;
	int month;
	int day;
};

struct time_struct
{
	int HH;
	int mm;
	int ss;
	int ms;
};
#pragma pack(pop)

class DateTime
{
public:
	DateTime() {}
	DateTime(int y, int m, int d, int H, int M, int S, int MS);
	date_struct GetDateStruct() const;
	time_struct GetTimeStruct() const;
	std::string GetDate();
	std::string GetTime();
	std::string GetDateTime();
	int year()const;
	int month()const;
	int day()const;
	int hour()const;
	int minute()const;
	int second()const;
	int millisecond()const;
private:
	int year_{ 0 };
	int month_{ 0 };
	int day_{ 0 };
	int hour_{ 0 };
	int minute_{ 0 };
	int second_{ 0 };
	int msecond_{ 0 };
};