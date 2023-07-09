#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <memory>
#include "Location.hpp"

class Server {
    public:
        int listenPort;
        std::string serverName;
        std::string host;
        std::string root;
        std::string indexFile;
        std::vector<Location> locations;
};
#endif