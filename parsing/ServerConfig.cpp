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


std::string ServerConfig::GetHostIp() const
{
    return hostIp;
}
void ServerConfig::SetHostIp(std::string hostIp)
{
    ServerConfig::hostIp = hostIp;
}
std::string ServerConfig::GetRoot() const
{
    return root;
}

void ServerConfig::SetRoot(std::string root)
{
    ServerConfig::root = root;
}
std::string ServerConfig::GetIndexFile() const
{
    return indexFile;
}

void ServerConfig::SetIndexFile(std::string indexFile)
{
    ServerConfig::indexFile = indexFile;
}
std::vector<LocationConfig> ServerConfig::GetLocations() const
{
    return locations;
}

void ServerConfig::AddLocations(LocationConfig location)
{
    ServerConfig::locations.push_back(location);
}

std::map<int, string> ServerConfig::GetError_pages() const
{
    return error_pages;
}

void ServerConfig::AddError_page(int errorCode, std::string error_page)
{
    ServerConfig::error_pages[errorCode] = error_page;
}
std::string ServerConfig::GetListenPort() const
{
    return listenPort;
}

void ServerConfig::SetListenPort(std::string listenPort)
{
    ServerConfig::listenPort = listenPort;
}

void ServerConfig::SetServerName(std::string serverName)
{
    ServerConfig::serverName = serverName;
}
std::string ServerConfig::GetServerName() const
{
    return serverName;
}

std::string ServerConfig::GetMaxBodySize() const
{
    return maxBodySize;
}

void ServerConfig::SetMaxBodySize(const std::string& max_body_size)
{
    this->maxBodySize = max_body_size;
}


void ServerConfig::print()
{
    std::cout << "\tServer: " << GetServerName() << std::endl;

    if (!ServerConfig::GetHostIp().empty())
        std::cout << "\tHostIp: " << GetHostIp() << std::endl;

    if (!ServerConfig::GetIndexFile().empty())
        std::cout << "\tIndexFile: " << GetIndexFile() << std::endl;

    if (!ServerConfig::GetListenPort().empty())
        std::cout << "\tListenPort: " << GetListenPort() << std::endl;

    if (!ServerConfig::GetRoot().empty())
        std::cout << "\tRoot: " << GetRoot() << std::endl;

    if (!ServerConfig::GetMaxBodySize().empty())
        std::cout << "\tMaxBodySize: " << GetMaxBodySize() << std::endl;

    std::vector<LocationConfig> locations = GetLocations();
    if (locations.size() > 0)
    {
        for (uint32_t i = 0; i < locations.size(); i++)
        {
            locations[i].print();
        }
    }

    std::map<int, string> error_pages = GetError_pages();
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
