#include "ServerConfig.hpp"


std::string _getcwd_str(void)
{
	char        buffer[FILENAME_MAX];
	std::string cwd;

	if (getcwd(buffer, sizeof(buffer)) == NULL)
		return ("");
	//cwd = buffer;
	return (buffer);
}

const std::string	ServerConfig::cwd = _getcwd_str();//std::filesystem::current_path();

ServerConfig::ServerConfig()//: maxBodySize(0)
{
}


std::string ServerConfig::GetHostIp() const
{
	return this->hostIp;
}
void ServerConfig::SetHostIp(std::string hostIp)
{
	this->hostIp = hostIp;
}
std::string ServerConfig::GetRoot() const
{
	return this->root;
}
const std::vector<std::string>& ServerConfig::GetSplitRoot() const
{
	return (this->split_root);
}

void ServerConfig::SetRoot(std::string root)
{
	this->root = root;
	split_string(root, '/', this->split_root);
}
std::string ServerConfig::GetIndexFile() const
{
	return this->indexFile;
}

void ServerConfig::SetIndexFile(std::string indexFile)
{
	this->indexFile = indexFile;
}
const std::vector<LocationConfig>&  ServerConfig::GetLocations() const
{
	return this->locations;
}

void ServerConfig::AddLocations(const LocationConfig& location)
{
	this->locations.push_back(location);
}

const std::map<int, std::string>& ServerConfig::GetError_pages() const
{
	return error_pages;
}

void ServerConfig::AddError_page(int errorCode, std::string error_page)
{
	this->error_pages[errorCode] = error_page;
}

std::string ServerConfig::GetListenPort() const
{
	return this->listenPort;
}

void ServerConfig::SetListenPort(const std::string& listenPort)
{
	this->listenPort = listenPort;
}
bool ServerConfig::AddListenPort(const std::string& listenPort)
{
	size_t		pos, pos2;
	std::string	parsed_listen;

	//if (!this->listenPort.empty())
	//	this->listenPort += ",";

	pos = listenPort.find_first_of("0123456789:.");
	pos2 = listenPort.find_first_not_of("0123456789:.", pos);
	if (pos2 == std::string::npos)
		parsed_listen = listenPort.substr(pos);
	else
		parsed_listen = listenPort.substr(pos, pos2 - pos);
	
	if (this->listenPort.find(parsed_listen) != std::string::npos
		|| this->listenPort.find("0.0.0.0:" + parsed_listen) != std::string::npos)
	{
		std::cout << "Current full listen port string : " << this->listenPort << std::endl;
		return (false);
	}
	else
	{
		std::cout << "Listen port:" << this->listenPort << std::endl;
		if (!this->listenPort.empty())
			this->listenPort += ",";
		if (parsed_listen.find(':') == std::string::npos)
			this->listenPort += "0.0.0.0:" + parsed_listen;
		else
			this->listenPort += parsed_listen;
		return (true);
	}
	//this->listenPort += parsed_listen;
}


void ServerConfig::SetServerName(std::string serverName)
{
	this->serverName = serverName;
}
std::string ServerConfig::GetServerName() const
{
	return serverName;
}

std::string	ServerConfig::GetMaxBodySize() const
//size_t	ServerConfig::GetMaxBodySize() const
{
	return maxBodySize;
}

void	ServerConfig::SetMaxBodySize(const std::string& maxBodySize)
//int	ServerConfig::SetMaxBodySize(const std::string& maxBodySize)
{
	// size_t	temp;
	// std::vector<std::string>	trimed_str;

	// split_string(maxBodySize, ' ', trimed_str);
	// if (trimed_str.size() == 0)
	// 	return (Logger::log(LOG_ERROR, "max_body_size has no value."), -1);

	// if (!is_all_digits(trimed_str[0]))
	// 	return (Logger::log(LOG_ERROR, "max_body_size should contain only digits."), -1);

	// temp = std::stol(trimed_str[0]);
	// this->maxBodySize = temp;
	// return (0);
	this->maxBodySize = maxBodySize;
}


void ServerConfig::print() const
{
	std::cout << "\tServer: " << GetServerName() << std::endl;

	if (!ServerConfig::GetHostIp().empty())
		std::cout << "\tHostIp: " << GetHostIp() << std::endl;

	if (!ServerConfig::GetIndexFile().empty())
		std::cout << "\tIndexFile: " << GetIndexFile() << std::endl;

	if (!ServerConfig::GetListenPort().empty())
		std::cout << "\tListenPort: " << GetListenPort() << std::endl;

	if (!ServerConfig::GetRoot().empty())
		std::cout << "\tRoot: " << GetRoot() << std::endl;

	if (!ServerConfig::GetMaxBodySize().empty())
		std::cout << "\tMaxBodySize: " << GetMaxBodySize() << std::endl;

	std::vector<LocationConfig> locations = GetLocations();
	if (locations.size() > 0)
	{
		for (size_t i = 0; i < locations.size(); i++)
		{
			locations[i].print();
		}
	}

	std::map<int, string> error_pages = GetError_pages();
	if (error_pages.size() > 0)
	{
		std::cout << "\tError_pages: \n";
		std::map<int, string>::iterator it;
		for (it = error_pages.begin(); it!= error_pages.end(); ++it)
		{
			std::cout << "\t\t " << it->first << "  " << it->second << std::endl;
		}
	}
	std::cout << "-------------------------------------------\n" << std::endl;
}

std::ostream&    operator<<(std::ostream& o, const ServerConfig& cfg)
{
	(void)o;
	cfg.print();
	return (o);
}


//int ServerConfig::getBestLocationMatch(const std::string& path) const {
const LocationConfig* ServerConfig::getBestLocationMatch(const std::string& path) const {
	size_t biggest_match = 0;
	const std::vector<LocationConfig>& locations = GetLocations();
	const LocationConfig*   best_match = NULL;
//    int bestMatch=-1;
//    int i=0;
	for (std::vector<LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		std::cout << "Best match path : " << path << " vs *it : " << it->GetPath() << std::endl;
		if (path.find(it->GetPath()) == 0)
		{
			if (it->GetPath() == "/" || path.length() == it->GetPath().length() || path[it->GetPath().length()] == '/')
			{
				if (it->GetPath().length() > biggest_match)
				{
					biggest_match = it->GetPath().length();
					//bestMatch = i;
					best_match = &(*it);
				}
			}
		}
//       i++;
	}
	return best_match;
//    return bestMatch;
}

