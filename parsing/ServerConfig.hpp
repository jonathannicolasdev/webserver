#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include "LocationConfig.hpp"
#include "Headers.hpp"

class ServerConfig
{
public:
    ServerConfig();

    std::string GetHostIp() const
    {
        return hostIp;
    }
    void SetHostIp(std::string hostIp)
    {
        ServerConfig::hostIp = hostIp;
    }
    std::string GetRoot() const
    {
        return root;
    }

    void SetRoot(std::string root)
    {
        ServerConfig::root = root;
    }
    std::string GetIndexFile() const
    {
        return indexFile;
    }

    void SetIndexFile(std::string indexFile)
    {
        ServerConfig::indexFile = indexFile;
    }
    std::vector<LocationConfig> GetLocations() const
    {
        return locations;
    }

    void AddLocations(LocationConfig location)
    {
        ServerConfig::locations.push_back(location);
    }
    std::map<int, string> GetError_pages() const
    {
        return error_pages;
    }

    void AddError_page(int errorCode, std::string error_page)
    {
        ServerConfig::error_pages[errorCode] = error_page;
    }
    int GetListenPort() const
    {
        return listenPort;
    }

    void SetListenPort(int listenPort)
    {
        ServerConfig::listenPort = listenPort;
    }

    void SetServerName(std::string serverName)
    {
        ServerConfig::serverName = serverName;
    }
    std::string GetServerName() const
    {
        return serverName;
    }

private:
    int listenPort;
    std::string serverName;
    std::string hostIp;
    std::string root;
    std::string indexFile;
    std::vector<LocationConfig> locations;
    std::map<int, string> error_pages;
};
#endif