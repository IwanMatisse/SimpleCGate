#pragma once
#include "stdafx.h"
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "DateTime.h"

DateTime Now();

//read ini-file, convert the file to std::map by section names 
std::map<std::string, std::map<std::string, std::string>> ReadConfigFile(const std::string& path);

void ReadParameter(std::map<std::string, std::string> &map, const std::string& name, std::string &dest);

void ReadParameter(std::map<std::string, std::string> &map, const std::string& name, bool &dest);

