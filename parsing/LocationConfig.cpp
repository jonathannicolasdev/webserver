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


std::string LocationConfig::GetPath() const
{
    return path;
}
void LocationConfig::SetPath(const std::string& path)
{
    LocationConfig::path = path;
}

std::string LocationConfig::GetIndexFile() const
{
    return indexFile;
}

void LocationConfig::SetIndexFile(const std::string& indexFile)
{
    LocationConfig::indexFile = indexFile;
}
std::string LocationConfig::GetRoot() const
{
    return root;
}

void LocationConfig::SetRoot(const std::string& root)
{
    LocationConfig::root = root;
}
std::vector<std::string> LocationConfig::GetAllowMethods() const
{
    return allowMethods;
}

void LocationConfig::AddAllowMethods(const std::string& method)
{
    allowMethods.push_back(method);
}

const std::string& LocationConfig::GetReturnPath() const
{
    return returnPath;
}

void LocationConfig::SetReturnPath(const std::string& returnPath)
{
    LocationConfig::returnPath = returnPath;
}

std::string LocationConfig::GetAutoIndex() const
{
    return autoIndex;
}

void LocationConfig::SetAutoIndex(const std::string& autoIndex)
{
    LocationConfig::autoIndex = autoIndex;
}

std::vector<std::string> LocationConfig::GetCgiPaths() const
{
    return cgiPaths;
}
std::string LocationConfig::GetMaxBodySize() const
{
    return maxBodySize;
}

void LocationConfig::SetMaxBodySize(const std::string& maxBodySize)
{
    LocationConfig::maxBodySize = maxBodySize;
}
std::string LocationConfig::GetAllowDelete() const
{
    return allowDelete;
}

void LocationConfig::SetAllowDelete(const std::string& allowDelete)
{
    LocationConfig::allowDelete = allowDelete;
}
std::string LocationConfig::GetUpload() const
{
    return upload;
}

void LocationConfig::SetUpload(const std::string& upload)
{
    LocationConfig::upload = upload;
}

void LocationConfig::AddCgiPath(const std::string& cgiPath)
{
    LocationConfig::cgiPaths.push_back(cgiPath);
}
std::vector<std::string> LocationConfig::GetCgiExts() const
{
    return cgiExts;
}

void LocationConfig::AddCgiExt(const std::string& cgiExt)
{
    LocationConfig::cgiExts.push_back(cgiExt);
}

void LocationConfig::print() const
{
    std::cout << "\tLocation: " << LocationConfig::GetPath() << std::endl;

    std::cout << "here we go !" << std::endl;
    std::vector<std::string> allowMethods = LocationConfig::GetAllowMethods();
    if (allowMethods.size() > 0)
    {
        std::cout << "\t\tmethods: ";
        for (size_t i = 0; i < allowMethods.size(); i++)
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
        for (size_t i = 0; i < cgiPaths.size(); i++)
        {
            std::cout << " " << cgiPaths[i];
        }
        std::cout << std::endl;
    }
    std::vector<std::string> cgiExts = LocationConfig::GetCgiExts();
    if (cgiExts.size() > 0)
    {
        std::cout << "\t\tcgi_ext: ";
        for (size_t i = 0; i < cgiExts.size(); i++)
        {
            std::cout << " " << cgiExts[i];
        }
        std::cout << std::endl;
    }
}