#include "ParseXml.h"
#include <iostream>
ParseXml::ParseXml() 
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("keyboard_config.xml");	
    pugi::xml_node tools = doc.child("configuration").child("Buttons");

    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling())
    {      
        int key = 0;
        int enableOpc = 0;
        std::string signalPath;      
        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute())
        {           
            std::string name = attr.name();            
            if (name == "EnableOPC") {
                enableOpc = atoi(attr.value());
            }
            else if (name == "SignalPath") {
               signalPath = attr.value();
            }
            else if (name == "Number") {
                key = atoi(attr.value());           
            }
        }
        signals[key].first = signalPath;
        signals[key].second = enableOpc;
    }
    tools = doc.child("configuration").child("OpcServer");
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling())
    {
        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute())
        {
            std::string name = attr.name();
            config.push_back(attr.value());
            //std::cout << "ip - " << attr.value() << '\n';
 
        }
    }
    tools = doc.child("configuration");
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling())
    {
        int i = 0;
        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute())
        {            
            config.push_back(attr.value());
        }
    }
}
ParseXml::~ParseXml() 
{
    std::cout << "DELETE PARSE XML\n";
}

void ParseXml::getSignalMap(std::map<int, std::pair<std::string, int>>& temp)
{
    if (signals.size() > 0)
        temp = signals;

}

void ParseXml::getConfigList(std::vector<std::string> &temp)
{
    if (config.size() > 0) 
       temp = config;
}