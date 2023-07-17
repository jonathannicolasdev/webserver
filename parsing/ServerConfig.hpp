#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include "LocationConfig.hpp"
#include "Headers.hpp"


class ServerConfig {
    public:
        int listenPort;
        std::string serverName;
        std::string hostIp;
        std::string root;
        std::string indexFile;
        std::vector<LocationConfig> locations;
        std::map<int,string> error_pages;
};
#endif