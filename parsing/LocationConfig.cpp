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
    std::cout << "\tLocation: " << LocationConfig::GetPath() << std::endl;

    std::vector<std::string> allowMethods = LocationConfig::GetAllowMethods();
    if (allowMethods.size() > 0)
    {
        std::cout << "\t\tmethods: ";
        for (int i = 0; i < allowMethods.size(); i++)
        {
            std::cout << " " << allowMethods[i];
        }
        std::cout << std::endl;
    }

    if (!LocationConfig::GetIndexFile().empty())
        std::cout << "\t\tindex: " << LocationConfig::GetIndexFile() << std::endl;

    if (!LocationConfig::GetRoot().empty())
        std::cout << "\t\troot: " << LocationConfig::GetRoot() << std::endl;

    if (!LocationConfig::GetReturnPath().empty())
        std::cout << "\t\treturn: " << LocationConfig::GetReturnPath() << std::endl;

    if (!LocationConfig::GetAutoIndex().empty())
        std::cout << "\t\tautoindex: " << LocationConfig::GetAutoIndex() << std::endl;

    if (!LocationConfig::GetMaxBodySize().empty())
        std::cout << "\t\tMaxBodySize: " << LocationConfig::GetMaxBodySize() << std::endl;

   if (!LocationConfig::GetAllowDelete().empty())
        std::cout << "\t\tAllowDelete: " << LocationConfig::GetAllowDelete() << std::endl;

   if (!LocationConfig::GetUpload().empty())
        std::cout << "\t\tUpload: " << LocationConfig::GetUpload() << std::endl;

    std::vector<std::string> cgiPaths = LocationConfig::GetCgiPaths();
    if (cgiPaths.size() > 0)
    {
        std::cout << "\t\tcgi_path: ";
        for (int i = 0; i < cgiPaths.size(); i++)
        {
            std::cout << " " << cgiPaths[i];
        }
        std::cout << std::endl;
    }
    std::vector<std::string> cgiExts = LocationConfig::GetCgiExts();
    if (cgiExts.size() > 0)
    {
        std::cout << "\t\tcgi_ext: ";
        for (int i = 0; i < cgiExts.size(); i++)
        {
            std::cout << " " << cgiExts[i];
        }
        std::cout << std::endl;
    }
}