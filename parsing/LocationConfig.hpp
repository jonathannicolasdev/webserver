#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <iostream>
#include <vector>

class LocationConfig {
    public:
        LocationConfig();
        std::string path;
        std::vector<std::string> allowMethods;
        bool    autoIndex;
        std::string indexFile;
        std::string return_path;
        std::string root;
        std::vector<std::string> cgi_paths;
        std::vector<std::string> cgi_ext;
};

#endif