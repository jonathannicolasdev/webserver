#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <iostream>
#include <vector>

class LocationConfig
{
public:
    LocationConfig();
    void print();

    std::string GetPath() const
    {
        return path;
    }

    void SetPath(std::string path)
    {
        LocationConfig::path = path;
    }
    std::string GetIndexFile() const
    {
        return indexFile;
    }

    void SetIndexFile(std::string indexFile)
    {
        LocationConfig::indexFile = indexFile;
    }
    std::string GetRoot() const
    {
        return root;
    }

    void SetRoot(std::string root)
    {
        LocationConfig::root = root;
    }
    std::vector<std::string> GetAllowMethods() const
    {
        return allowMethods;
    }

    void AddAllowMethods(std::string method)
    {
        LocationConfig::allowMethods.push_back(method);
    }

    std::string GetReturnPath() const
    {
        return returnPath;
    }

    void SetReturnPath(std::string returnPath)
    {
        LocationConfig::returnPath = returnPath;
    }

    std::string GetAutoIndex() const
    {
        return autoIndex;
    }

    void SetAutoIndex(std::string autoIndex)
    {
        LocationConfig::autoIndex = autoIndex;
    }

    std::vector<std::string> GetCgiPaths() const
    {
        return cgiPaths;
    }
    std::string GetMaxBodySize() const
    {
        return maxBodySize;
    }

    void SetMaxBodySize(std::string maxBodySize)
    {
        LocationConfig::maxBodySize = maxBodySize;
    }
    std::string GetAllowDelete() const
    {
        return allowDelete;
    }

    void SetAllowDelete(std::string allowDelete)
    {
        LocationConfig::allowDelete = allowDelete;
    }
    std::string GetUpload() const
    {
        return upload;
    }

    void SetUpload(std::string upload)
    {
        LocationConfig::upload = upload;
    }

    void AddCgiPath(std::string cgiPath)
    {
        LocationConfig::cgiPaths.push_back(cgiPath);
    }
    std::vector<std::string> GetCgiExts() const
    {
        return cgiExts;
    }

    void AddCgiExt(std::string cgiExt)
    {
        LocationConfig::cgiExts.push_back(cgiExt);
    }

private:
    std::string path;
    std::vector<std::string> allowMethods;
    std::string autoIndex;
    std::string indexFile;
    std::string returnPath;
    std::string root;
    std::vector<std::string> cgiPaths;
    std::vector<std::string> cgiExts;
    std::string maxBodySize; // max_body_size
    std::string upload;
    std::string allowDelete; // allow_delete on;

    // limit_except, auth
};

#endif