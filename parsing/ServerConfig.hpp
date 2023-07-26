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

    std::string GetHostIp() const;
    void        SetHostIp(std::string hostIp);
    std::string GetRoot() const;
    void        SetRoot(std::string root);
    std::string GetIndexFile() const;
    void        SetIndexFile(std::string indexFile);
    std::vector<LocationConfig> GetLocations() const;
    void        AddLocations(LocationConfig location);
    std::map<int, string> GetError_pages() const;
    void        AddError_page(int errorCode, std::string error_page);
    std::string GetListenPort() const;
    void        SetListenPort(std::string listenPort);
    void        SetServerName(std::string serverName);
    std::string GetServerName() const;
    std::string GetMaxBodySize() const;
    void        SetMaxBodySize(const std::string& max_body_size);

    void print();

private:
    std::string listenPort;
    std::string        serverName;
    std::string        hostIp;
    std::string        root;
    std::string        indexFile;
    std::vector<LocationConfig> locations;
    std::map<int, string>       error_pages;
    std::string                 maxBodySize; // max_body_size
};
#endif