#include "ConfigBuilder.hpp"
#include "LocationConfig.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

LocationConfig::LocationConfig()
{
}

void LocationConfig::print()
{
    std::cout << "name location: " << LocationConfig::GetPath() << std::endl;

    std::vector<std::string> allowMethods = LocationConfig::GetAllowMethods();
    if (allowMethods.size() > 0)
    {
        std::cout << "methods: ";
        for (int i = 0; i < allowMethods.size(); i++)
        {
            std::cout << " " << allowMethods[i];
        }
        std::cout << std::endl;
    }

    if (!LocationConfig::GetIndexFile().empty())
        std::cout << "index: " << LocationConfig::GetIndexFile() << std::endl;

    if (!LocationConfig::GetRoot().empty())
        std::cout << "root: " << LocationConfig::GetRoot() << std::endl;

    if (!LocationConfig::GetReturnPath().empty())
        std::cout << "return: " << LocationConfig::GetReturnPath() << std::endl;

    if (!LocationConfig::GetAutoIndex().empty())
        std::cout << "autoindex: " << LocationConfig::GetAutoIndex() << std::endl;

    std::vector<std::string> cgiPaths = LocationConfig::GetCgiPaths();
    if (cgiPaths.size() > 0)
    {
        std::cout << "cgi_path: ";
        for (int i = 0; i < cgiPaths.size(); i++)
        {
            std::cout << " " << cgiPaths[i];
        }
        std::cout << std::endl;
    }
    std::vector<std::string> cgiExts = LocationConfig::GetCgiExts();
    if (cgiExts.size() > 0)
    {
        std::cout << "cgi_ext: ";
        for (int i = 0; i < cgiExts.size(); i++)
        {
            std::cout << " " << cgiExts[i];
        }
        std::cout << std::endl;
    }
}