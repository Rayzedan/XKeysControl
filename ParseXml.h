#pragma once
#include<pugixml.hpp>
#include<string>
#include<map>
class ParseXml
{
public:
	ParseXml();
	virtual ~ParseXml();
	void getSignalMap(std::map<int, std::pair<std::string, int>>& temp);
private:
	std::map<int, std::pair<std::string, int>> signals;
};

