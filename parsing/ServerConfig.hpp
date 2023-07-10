#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <iostream>
#include <vector>
#include <memory>
#include "LocationConfig.hpp"
#include "Headers.hpp"


class Server {
    public:
        int listenPort;
        std::string serverName;
        std::string hostIp;
        std::string root;
        std::string indexFile;
        std::vector<Location> locations;
        map<int,string> error_pages;
};
#endif