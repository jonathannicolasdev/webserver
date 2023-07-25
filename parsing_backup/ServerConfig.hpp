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
        hostIp = hostIp;
    }
    std::string GetRoot() const
    {
        return root;
    }

    void SetRoot(std::string root)
    {
        root = root;
    }
    std::string GetIndexFile() const
    {
        return indexFile;
    }

    void SetIndexFile(std::string indexFile)
    {
        indexFile = indexFile;
    }
    std::vector<LocationConfig> GetLocations() const
    {
        return locations;
    }

    void SetLocations(std::vector<LocationConfig> locations)
    {
        locations = locations;
    }
    std::map<int, string> GetError_pages() const
    {
        return error_pages;
    }

    void SetError_pages(std::map<int, string> error_pages)
    {
        error_pages = error_pages;
    }
    int GetListenPort() const
    {
        return listenPort;
    }

    void SetListenPort(int listenPort)
    {
        listenPort = listenPort;
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