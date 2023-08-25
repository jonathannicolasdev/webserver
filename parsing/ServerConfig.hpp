#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <memory>
#include <unistd.h>
#include "ConfigBuilder.hpp"
#include "LocationConfig.hpp"
#include "Headers.hpp"

#define	DEFAULT_MAX_BODY_SIZE "10000000"

class ServerConfig
{
public:

	static const std::string	cwd;

	ServerConfig();

	std::string GetHostIp() const;	
	void		SetHostIp(std::string hostIp);
	std::string GetRoot() const;
	void		SetRoot(std::string root);
	const std::vector<std::string>& GetSplitRoot() const;
	std::string GetIndexFile() const;
	void		SetIndexFile(std::string indexFile);
	const std::vector<LocationConfig>& GetLocations() const;
	void		AddLocations(const LocationConfig& location);
	const std::map<int, std::string>& GetError_pages() const;
	void		AddError_page(int errorCode, std::string error_page);
	std::string GetListenPort() const;
	void		SetListenPort(const std::string& listenPort);
	bool		AddListenPort(const std::string& listenPort);
	void		SetServerName(std::string serverName);
	std::string GetServerName() const;
	std::string	GetMaxBodySize() const;
	void		SetMaxBodySize(const std::string& max_body_size);
	const LocationConfig* getBestLocationMatch(const std::string& path) const;

	void print() const;

private:
	std::string					listenPort;
	std::string					serverName;
	std::string					hostIp;
	std::string					root;
	std::vector<std::string>	split_root;
	std::string					indexFile;
	std::vector<LocationConfig> locations;
	std::map<int, std::string>		error_pages;
	std::string				maxBodySize; // max_body_size
};

std::ostream&    operator<<(std::ostream& o, const ServerConfig& cfg);
#endif