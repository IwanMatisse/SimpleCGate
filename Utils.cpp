#include "stdafx.h"
#include "Utils.h"
#include <time.h>

namespace simple_cgate
{
	DateTime Now()
	{
		time_t date;
		time(&date);
		struct tm t;
		localtime_s(&t, &date);
		return DateTime(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, 0);
	}

	std::map<std::string, std::map<std::string, std::string>> ReadConfigFile(const std::string& path)
	{
		std::map<std::string, std::map<std::string, std::string>> res;
		std::ifstream file(path);
		std::string line;
		std::string cur_section{ "" };
		while (std::getline(file, line))
		{
			if (line[0] == '[')
			{
				cur_section = line.substr(1, line.size() - 2);
				continue;
			}
			if (cur_section.size() > 1)
			{
				auto t = line.find('=');
				std::string left = line.substr(0, t);
				std::string right = line.substr(t + 1, line.size() - 1);

				res[cur_section][left] = right;
			}
		}
		return res;
	}

	void ReadParameter(std::map<std::string, std::string> &map,
		const std::string& name,
		std::string &dest)
	{
		if (map.find(name) != map.end())
			dest = map[name];
	}

	void ReadParameter(std::map<std::string, std::string> &map,
		const std::string& name,
		bool &dest)
	{
		if (map.find(name) != map.end())
			dest = std::stoi(map[name]) != 0;
	}
}//namespace simple_cgate