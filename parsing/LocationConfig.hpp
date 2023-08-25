#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include "ServerConfig.hpp"
#include "webserv_utils.hpp"

class ServerConfig;

class LocationConfig
{
public:
	LocationConfig();
	LocationConfig(ServerConfig& cfg);
	LocationConfig(const LocationConfig& other);
	LocationConfig&	operator=(const LocationConfig& other);

	void print() const;

	std::string 		GetPath() const;
	void 				SetPath(const std::string& path);

	std::string 		GetIndexFile() const;
	void				SetIndexFile(const std::string& indexFile);
	
	std::string 		GetRoot() const;
	void				SetRoot(const std::string& root);
	const std::vector<std::string>& GetSplitRoot() const;
	
	const std::set<std::string>& GetAllowMethods() const;
	void				AddAllowMethods(const std::string& method);
	bool				IsAllowedMethod(const std::string& method) const;

	const std::string&	GetReturnPath() const;
	void 				SetReturnPath(const std::string& returnPath);
	std::string 		GetAutoIndex() const;
	void 				SetAutoIndex(const std::string& autoIndex);
	std::string 		GetMaxBodySize() const;
	void				SetMaxBodySize(const std::string& maxBodySize);
	bool				GetAllowDelete() const;
	void 				SetAllowDelete(const std::string& allowDelete);
	bool 				GetAllowDownload(void) const;
	void 				SetAllowDownload(const std::string& allowDownload);
	std::string 		GetUpload() const;
	void 				SetUpload(const std::string& upload);

	void 						AddCgiPath(const std::string& cgiPath);
	const std::vector<std::string>&	GetCgiPaths() const;
	const std::vector<std::string>& GetCgiExts() const;
	void 						AddCgiExt(const std::string& cgiExt);

	const ServerConfig&	GetServerConfig() const;

private:
	ServerConfig&				srv_cfg;
	std::string					path;
	std::set<std::string>		allowMethods;
	std::string					autoIndex;
	std::string					indexFile;
	std::string					returnPath;
	std::string					root;
	std::vector<std::string>	split_root;
	std::vector<std::string>	cgiPaths;
	std::vector<std::string>	cgiExts;
	std::string maxBodySize; // max_body_size
	std::string upload;
	std::string allowDelete; // allow_delete on;
	std::string allowDownload; // allow_delete on;
};

#endif