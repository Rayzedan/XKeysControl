#pragma once
#include<pugixml.hpp>
#include<string>
#include<map>
<<<<<<< HEAD
#include<vector>
=======
>>>>>>> 4df2694 (init project)
class ParseXml
{
public:
	ParseXml();
	virtual ~ParseXml();
	void getSignalMap(std::map<int, std::pair<std::string, int>>& temp);
<<<<<<< HEAD
	void getConfigList(std::vector<std::string>& temp);
private:
	int convertIndexButton(int index);
	std::map<int, std::pair<std::string, int>> signals;
	std::vector<std::string> config;
=======
private:
	std::map<int, std::pair<std::string, int>> signals;
>>>>>>> 4df2694 (init project)
};

