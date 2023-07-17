#ifndef CONFIGBUILDER_HPP
#define CONFIGBUILDER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

class ConfigBuilder
{
public:
    ConfigBuilder();
    string readConfigFile(const std::string& filename);
    string cleanSpaces(const std::string &content);
    string cleanComments(const std::string &content);
    LocationConfig parseLocation(string content);
    ServerConfig parseServer(string content);
    void parseConfigFile(const std::string filename);
    void setListenPort(int port);
    void setServerName(const std::string &name);
    void setHost(const std::string &host);
    void setRoot(const std::string &root);
    void setIndexFile(const std::string &indexFile);
    void addLocation(const ServerConfig &location);
    ServerConfig build();

private:
    ServerConfig server;
};

#endif // CONFIGBUILDER_HPP