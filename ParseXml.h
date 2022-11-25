#pragma once
#include<pugixml.hpp>
#include<string>
#include<map>
#include<vector>
class ParseXml
{
public:
	ParseXml();
	virtual ~ParseXml();
	void getSignalMap(std::map<int, std::pair<std::string, int>>& temp);
	void getConfigList(std::vector<std::string>& temp);
private:
	int convertIndexButton(int index);
	std::map<int, std::pair<std::string, int>> signals;
	std::vector<std::string> config;
};

