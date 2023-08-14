#ifndef CONFIGBUILDER_HPP
#define CONFIGBUILDER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

class ServerConfig;

class ConfigBuilder
{
public:
    ConfigBuilder();
    std::vector<ServerConfig> parseConfigFile(const std::string& filename);

private:
    std::string readConfigFile(const std::string& filename);
    std::string cleanSpaces(const std::string &content);
    std::string cleanComments(const std::string &content);
    LocationConfig parseLocation(std::string content);
    ServerConfig parseServer(std::string content);  
    void setListenPort(int port);
    void setServerName(const std::string &name);
    void setHost(const std::string &host);
    void setRoot(const std::string &root);
    void setIndexFile(const std::string &indexFile);
    void addLocation(const ServerConfig &location);
    ServerConfig build();
};

#endif // CONFIGBUILDER_HPP