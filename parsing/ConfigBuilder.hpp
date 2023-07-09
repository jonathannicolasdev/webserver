#ifndef CONFIGBUILDER_HPP
#define CONFIGBUILDER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Server.hpp"

class ConfigBuilder {
public:
    ConfigBuilder();
    void parseConfigFile(const std::string& filename);
    void setListenPort(int port);
    void setServerName(const std::string& name);
    void setHost(const std::string& host);
    void setRoot(const std::string& root);
    void setIndexFile(const std::string& indexFile);
    void addLocation(const Location& location);
    Server build();
    
private:
    Server server;
};

#endif // CONFIGBUILDER_HPP