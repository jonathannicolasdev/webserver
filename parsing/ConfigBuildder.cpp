#include "ConfigBuilder.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

void ConfigBuilder::parseConfigFile(const std::string& filename) {
    std::ifstream configFile(filename);
    std::string line;

    if (!configFile.is_open()) {
        std::cerr << "Error opening config file: " << filename << std::endl;
        return;
    }

    Location currentLocation;
    bool inLocationBlock = false;

    while (std::getline(configFile, line)) {
        // Remove leading and trailing whitespaces from the line
        line = trim(line);

        if (line.empty() || line[0] == '#') {
            // Skip empty lines and comments
            continue;
        } else if (line.find("server {") != std::string::npos) {
            // Start of server block
            server = Server();
            continue;
        } else if (line.find("}") != std::string::npos) {
            // End of server block or location block
            if (inLocationBlock) {
                // End of location block, add currentLocation to the server
                server.addLocation(currentLocation);
                currentLocation = Location();
                inLocationBlock = false;
            }
            continue;
        }

        // Parse directives within server or location block
        std::istringstream iss(line);
        std::string directive, value;
        std::getline(iss, directive, ' ');
        std::getline(iss, value);

        if (!inLocationBlock) {
            // Directives within server block
            if (directive == "listen") {
                int port = std::stoi(value);
                setListenPort(port);
            } else if (directive == "server_name") {
                setServerName(value);
            } else if (directive == "host") {
                setHost(value);
            } else if (directive == "root") {
                setRoot(value);
            } else if (directive == "index") {
                setIndexFile(value);
            }
        } else {
            // Directives within location block
            if (directive == "location") {
                currentLocation.setPath(value);
            } else if (directive == "allow_methods") {
                std::vector<std::string> methods = split(value, ' ');
                currentLocation.setAllowedMethods(methods);
            } else if (directive == "autoindex") {
                bool autoindex = (value == "on");
                currentLocation.setAutoindex(autoindex);
            }
            // Add more directives as needed
        }
    }

    configFile.close();
}