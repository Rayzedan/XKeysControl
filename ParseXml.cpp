#include "ParseXml.h"
#include <Windows.h>
#include <algorithm>

ParseXml::ParseXml() 
{

}
ParseXml::~ParseXml() 
{

}

void ParseXml::getSignalMap(std::map<int, std::pair<std::string, int>>& temp)
{
    if (signals.size() > 0)
        temp = signals;

}

void ParseXml::getConfigList(std::vector<std::string> &temp)
{
    if (config.size() > 0) {
        temp = config;
    }
    else {
        config.push_back("opc.tcp://localhost:62544");
        config.push_back("5");
        config.push_back("30");
        temp = config;
    }
}

int ParseXml::getConfigFile()
{
    TCHAR tempPath[MAX_PATH]; //or wchar_t * buffer;
    GetModuleFileName(NULL, tempPath, MAX_PATH);
    std::string path(tempPath);
    std::string xmlPath = "\\keyboard_config.xml";
    size_t index = path.rfind('\\');
    if (index != std::string::npos) {
        path.replace(index,xmlPath.size()+2, xmlPath);
        std::string::iterator iter = std::find(path.begin(), path.end(), '/');
        if (iter < path.end())
            std::replace(++iter, path.end(), '/', '\\');
    }
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());
    if (result) {
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
                    key = convertIndexButton(atoi(attr.value()));
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
        return 0;
    }
    else {
        return -1;
    }
    
    return 0;
}

int ParseXml::convertIndexButton(int index)
{
    //Key Index (in decimal)
    //Bank 1
    //Columns-->
    //  0   8   16  24  32  40  48  56  64  72
    //  1   9   17  25  33  41  49  57  65  73

    //  3   11      27  35  43  51      67  75
    //  4   12      28  36  44  52      68  76
    //  5   13      29  37  45  53      69  77
    //  6   14      30  38  46  54      70  78
    //  7   15      31  39  47  55      71  79

    //Bank 2
    //Columns-->
    //  80	88	96	104	112	120	128	136	144	152
    //  81	89	97	105	113	121	129	137	145	153

    //  83	91		107	115	123	131		147	155
    //  84	92		108	116	124	132		148	156
    //  85	93		109	117	125	133		149	157
    //	86	94		110	118	126	134		150	158
    //  87  95      111 119 127 135     151 159

    switch (index)
    {
    case 1:
        return 0;
        break;
    case 2:
        return 8;
        break;
    case 3:
        return 16;
        break;
    case 4:
        return 24;
        break;
    case 5:
        return 32;
        break;
    case 6:
        return 40;
        break;
    case 7:
        return 48;
        break;
    case 8:
        return 56;
        break;
    case 9:
        return 64;
        break;
    case 10:
        return 72;
        break;
    case 11:
        return 1;
        break;
    case 12:
        return 9;
        break;
    case 13:
        return 17;
        break;
    case 14:
        return 25;
        break;
    case 15:
        return 33;
        break;
    case 16:
        return 41;
        break;
    case 17:
        return 49;
        break;
    case 18:
        return 57;
        break;
    case 19:
        return 65;
        break;
    case 20:
        return 73;
        break;
    case 21:
        return 3;
        break;
    case 22:
        return 11;
        break;
    case 23:
        return 27;
        break;
    case 24:
        return 35;
        break;
    case 25:
        return 43;
        break;
    case 26:
        return 51;
        break;
    case 27:
        return 67;
        break;
    case 28:
        return 75;
        break;
    case 29:
        return 4;
        break;
    case 30:
        return 12;
        break;
    case 31:
        return 28;
        break;
    case 32:
        return 36;
        break;
    case 33:
        return 44;
        break;
    case 34:
        return 52;
        break;
    case 35:
        return 68;
        break;
    case 36:
        return 74;
    case 37:
        return 5;
        break;
    case 38:
        return 13;
        break;
    case 39:
        return 29;
        break;
    case 40:
        return 37;
        break;
    case 41:
        return 45;
        break;
    case 42:
        return 53;
        break;
    case 43:
        return 69;
        break;
    case 44:
        return 77;
    case 45:
        return 6;
        break;
    case 46:
        return 14;
        break;
    case 47:
        return 30;
        break;
    case 48:
        return 38;
        break;
    case 49:
        return 46;
        break;
    case 50:
        return 54;
        break;
    case 51:
        return 70;
        break;
    case 52:
        return 78;
    case 53:
        return 7;
        break;
    case 54:
        return 15;
        break;
    case 55:
        return 31;
        break;
    case 56:
        return 39;
        break;
    case 57:
        return 47;
        break;
    case 58:
        return 55;
        break;
    case 59:
        return 71;
        break;
    case 60:
        return 79;
        break;
    default:
        return 0;
        break;
    }
}