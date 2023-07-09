#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>

class Location {
    public:
        std::string path;
        std::vector<std::string> allowMethods;
        bool    autoIndex;
        std::string indexFile;
};

#endif