#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "ConfigBuilder.hpp"

int main()
{
    ConfigBuilder configBuilder;// = ConfigBuilder();

    std::vector<ServerConfig> serverConfigs = configBuilder.parseConfigFile("parsing/config.txt");

    for(uint32_t i=0; i < serverConfigs.size();i++)
    {
        serverConfigs[i].print();       
    }

    return 0;
}