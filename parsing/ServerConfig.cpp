#include "ConfigBuilder.hpp"
#include "ServerConfig.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

ServerConfig::ServerConfig()
{
}

void ServerConfig::print()
{
    std::cout << "\tServer: " << ServerConfig::GetServerName() << std::endl;

    if (!ServerConfig::GetHostIp().empty())
        std::cout << "\tHostIp: " << ServerConfig::GetHostIp() << std::endl;

    if (!ServerConfig::GetIndexFile().empty())
        std::cout << "\tIndexFile: " << ServerConfig::GetIndexFile() << std::endl;

    if (!ServerConfig::GetListenPort().empty())
        std::cout << "\tListenPort: " << ServerConfig::GetListenPort() << std::endl;

    if (!ServerConfig::GetRoot().empty())
        std::cout << "\tRoot: " << ServerConfig::GetRoot() << std::endl;

    if (!ServerConfig::GetMaxBodySize().empty())
        std::cout << "\tMaxBodySize: " << ServerConfig::GetMaxBodySize() << std::endl;

    std::vector<LocationConfig> locations = ServerConfig::GetLocations();
    if (locations.size() > 0)
    {
        for (int i = 0; i < locations.size(); i++)
        {
            locations[i].print();
        }
    }

    std::map<int, string> error_pages = ServerConfig::GetError_pages();
    if (error_pages.size() > 0)
    {
        std::cout << "\tError_pages: \n";
        std::map<int, string>::iterator it;
        for (it = error_pages.begin(); it!= error_pages.end(); ++it)
        {
            std::cout << "\t\t " << it->first << "  " << it->second << std::endl;
        }
    }
    std::cout << "-------------------------------------------\n";
}
