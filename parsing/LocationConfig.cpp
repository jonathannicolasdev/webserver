#include "ConfigBuilder.hpp"
#include "LocationConfig.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>


LocationConfig::LocationConfig(ServerConfig& cfg): srv_cfg(cfg),
	maxBodySize(cfg.GetMaxBodySize())
{
	allowMethods.insert("GET");
}

LocationConfig::LocationConfig(const LocationConfig& other): srv_cfg(other.srv_cfg)
{
	if (this == &other)
		return ;
	srv_cfg = other.srv_cfg;
	path = other.path;
	allowMethods = other.allowMethods;
	autoIndex = other.autoIndex;
	indexFile = other.indexFile;
	returnPath = other.returnPath;
	root = other.root;
	split_root = other.split_root;
	cgiPaths = other.cgiPaths;
	cgiExts = other.cgiExts;
	maxBodySize = other.maxBodySize;
	upload = other.upload;
	allowDelete = other.allowDelete; // allow_delete on;
	return ;
}

LocationConfig&	LocationConfig::operator=(const LocationConfig& other)
{
	if (this == &other)
		return (*this);
	srv_cfg = other.srv_cfg;
	path = other.path;
	allowMethods = other.allowMethods;
	autoIndex = other.autoIndex;
	indexFile = other.indexFile;
	returnPath = other.returnPath;
	root = other.root;
	split_root = other.split_root;
	cgiPaths = other.cgiPaths;
	cgiExts = other.cgiExts;
	maxBodySize = other.maxBodySize; // max_body_size
	upload = other.upload;
	allowDelete = other.allowDelete; // allow_delete on;
	return (*this);
}


std::string LocationConfig::GetPath() const
{
	return path;
}
void LocationConfig::SetPath(const std::string& path)
{
	this->path = path;
}

std::string LocationConfig::GetIndexFile() const
{
	return indexFile;
}

void LocationConfig::SetIndexFile(const std::string& indexFile)
{
	if (indexFile.empty())
		return ;
	else if (indexFile[0] != '/')
		this->indexFile = std::string("/") + indexFile;
	else
		this->indexFile = indexFile;
}
std::string LocationConfig::GetRoot() const
{
	return root;
}
const std::vector<std::string>& LocationConfig::GetSplitRoot() const
{
	return (this->split_root);
}

void LocationConfig::SetRoot(const std::string& root)
{
	this->root = root;
	split_string(root, '/', this->split_root);
}

const std::set<std::string>& LocationConfig::GetAllowMethods() const
{
	return allowMethods;
}

void	LocationConfig::AddAllowMethods(const std::string& method)
{
	//allowMethods.push_back(method);
	allowMethods.insert(method);
}

bool	LocationConfig::IsAllowedMethod(const std::string& method) const
{
	return (std::find(allowMethods.begin(), allowMethods.end(), method) != allowMethods.end());
}


const std::string& LocationConfig::GetReturnPath() const
{
	return returnPath;
}

void LocationConfig::SetReturnPath(const std::string& returnPath)
{
	this->returnPath = returnPath;
}

std::string LocationConfig::GetAutoIndex() const
{
	return autoIndex;
}

void LocationConfig::SetAutoIndex(const std::string& autoIndex)
{
	this->autoIndex = autoIndex;
}

const	std::vector<std::string>& LocationConfig::GetCgiPaths() const
{
	return cgiPaths;
}
std::string LocationConfig::GetMaxBodySize() const
{
	return maxBodySize;
}

void	LocationConfig::SetMaxBodySize(const std::string& maxBodySize)
{
	this->maxBodySize = maxBodySize;
}
std::string LocationConfig::GetAllowDelete() const
{
	return allowDelete;
}

void LocationConfig::SetAllowDelete(const std::string& allowDelete)
{
	this->allowDelete = allowDelete;
}
std::string LocationConfig::GetUpload() const
{
	return upload;
}

void LocationConfig::SetUpload(const std::string& upload)
{
	this->upload = upload;
}

void LocationConfig::AddCgiPath(const std::string& cgiPath)
{
	this->cgiPaths.push_back(cgiPath);
}
const std::vector<std::string>& LocationConfig::GetCgiExts() const
{
	return cgiExts;
}

void LocationConfig::AddCgiExt(const std::string& cgiExt)
{
	this->cgiExts.push_back(cgiExt);
}

void LocationConfig::print() const
{
	std::cout << "\tLocation: " << LocationConfig::GetPath() << std::endl;

	std::cout << "here we go !" << std::endl;
	if (allowMethods.size() > 0)
	{
		std::set<std::string>::iterator	it;

		std::cout << "\t\tmethods: ";
		for (it=allowMethods.begin(); it != allowMethods.end(); ++it)
		{
			std::cout << " " << *it;
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

	//if (!LocationConfig::GetMaxBodySize())
	if (!LocationConfig::GetMaxBodySize().empty())
		std::cout << "\t\tMaxBodySize: " << LocationConfig::GetMaxBodySize() << std::endl;

	if (!LocationConfig::GetAllowDelete().empty())
		std::cout << "\t\tAllowDelete: " << LocationConfig::GetAllowDelete() << std::endl;

	if (!LocationConfig::GetUpload().empty())
		std::cout << "\t\tUpload: " << LocationConfig::GetUpload() << std::endl;

	const std::vector<std::string>& cgiPaths = LocationConfig::GetCgiPaths();
	if (cgiPaths.size() > 0)
	{
		std::cout << "\t\tcgi_path: ";
		for (size_t i = 0; i < cgiPaths.size(); i++)
		{
			std::cout << " " << cgiPaths[i];
		}
		std::cout << std::endl;
	}
	const std::vector<std::string>& cgiExts = LocationConfig::GetCgiExts();
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

const ServerConfig&	LocationConfig::GetServerConfig() const
{
	return (this->srv_cfg);
}
