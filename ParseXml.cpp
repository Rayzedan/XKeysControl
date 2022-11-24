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
                //std::cout << "enableOpc - " << enableOpc << std::endl;
            }
            else if (name == "SignalPath") {
               signalPath = attr.value();
               //std::cout << "signalPath - " << signalPath << std::endl;
            }
            else if (name == "Number") {
                key = atoi(attr.value());
                //std::cout << "KEY - " << key << std::endl;             
            }
        }
        signals[key].first = signalPath;
        signals[key].second = enableOpc;
        //std::cout << name
    }
}
ParseXml::~ParseXml() 
{
    //std::cout << "DELETE PARSE XML\n";
}

void ParseXml::getSignalMap(std::map<int, std::pair<std::string, int>>& temp)
{
    temp = signals;
}
