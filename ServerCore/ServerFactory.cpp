/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFactory.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 21:21:46 by marvin            #+#    #+#             */
/*   Updated: 2023/07/03 21:21:46 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerFactory.hpp"


bool	_parse_config_ports(const std::string& listen_ports,
	std::vector<std::string>& listen_addrs, std::vector<int>& ports)
{
	size_t			pos_mid = 0;
	std::ostringstream	os;
	std::string		sub, addr, port;
	int				num_port;
	std::vector<std::string>			split_listens;
	std::vector<std::string>::iterator	it;

	split_string(listen_ports, ',', split_listens);
	for (it=split_listens.begin(); it != split_listens.end(); ++it)
	{
		sub = *it;
		if ((pos_mid = sub.find(':')) == sub.npos)
			sub = "0.0.0.0:" + sub;
		pos_mid = sub.find(':');
		addr = sub.substr(0, pos_mid);
		port = sub.substr(pos_mid + 1);

		if (addr.empty() || port.empty())
			return (Logger::log(LOG_CRITICAL, std::string("Invalid listen port or listen address found in config. Address : ") + addr + ", port : " + port), false);

		if (inet_addr(addr.c_str()) == INADDR_NONE)
			return (Logger::log(LOG_ERROR, std::string("Configuration file listen address ") + addr + " is not a valid network ip address."), false);
		
		num_port = atoi(port.c_str());
		if (num_port < 1 || 65535 < num_port || !is_all_digits(port))
		{
			os << num_port;
			Logger::log(LOG_ERROR, std::string("Configuration file listen port ") + os.str() + " is not a valid port number.");
			return (false);
		}
		listen_addrs.push_back(addr);
		ports.push_back(num_port);
		std::cout << "listen addr: " << *(listen_addrs.end() - 1) << std::endl;
		std::cout << "listen port: " << *(ports.end() - 1) << std::endl;
	}
	if (ports.empty())
	{
		addr = "0.0.0.0";
		listen_addrs.push_back("0.0.0.0");
		ports.push_back(80);
	}
	return (true);
}

bool	_check_if_existing_server_with_same_interface_and_config(IServer *new_srv, const std::vector<AServerDispatchSwitch*>& built_servers)//const IServer **built_servers, const int nb_srvs)
{
	std::vector<AServerDispatchSwitch*>::const_iterator		srv_it = built_servers.begin();

	for (; srv_it != built_servers.end() - 1; ++srv_it)
	{
		if (*srv_it == new_srv)
		{
			//Logger::log(LOG_ERROR, "The Configuration file contains multiple servers with the same network interface and configuration.");
			Logger::log(LOG_WARNING, "The Configuration file contains a server configuration with multiple similar network interface. Bypassing construction.");
			return (true);
		}
	}
	return (false);
}

bool	_flush_servers_after_error(AServerCluster* clu, std::vector<AServerDispatchSwitch*>* built_servers)//IServer **built_servers)//, int nb_srvs)
{
	std::vector<AServerDispatchSwitch*>::iterator		srv_it = built_servers->begin();

	std::cerr << "Flushing ServerFactory Servers after bad config error." << std::endl;
	if (clu)
		delete clu;
	if (built_servers)
	{
		for (; srv_it != built_servers->end(); ++srv_it)
			delete *srv_it;
		built_servers->clear();
	}
	return (false);
}

bool	ServerFactory::create_servers_from_cfg(const ServerConfig& scfg, std::vector<AServerDispatchSwitch*>& built_servers)
{
	std::string			listen_str, serverName, rootDir;
	std::vector<std::string>	listen_addr;
	std::vector<int>			listen_ports;
	std::vector<std::string>::iterator	addr_it;// = listen_addr.begin();
	std::vector<int>::iterator			port_it;// = listen_ports.begin();

	listen_str = scfg.GetListenPort();

	if (!_parse_config_ports(listen_str, listen_addr, listen_ports))//, &nb_ports))
		return (false);
 	addr_it = listen_addr.begin();
 	port_it = listen_ports.begin();
	for (; port_it != listen_ports.end(); ++addr_it, ++port_it)//, ++(*nb_srvs))
	{
		std::cout << "Building server" << std::endl;
		std::cout << " - server name :	" << scfg.GetServerName() << std::endl;
		std::cout << " - root dir :		" << scfg.GetRoot() << std::endl;
		std::cout << " - ip addr  :		" << *addr_it << std::endl;
		std::cout << " - port	 :		" << *port_it << std::endl;
		built_servers.push_back(new ServerHTTP(scfg.GetServerName(), scfg.GetRoot(),
												*addr_it, *port_it, SRVHTTP_DEFAULT_TIMEOUT, &scfg));
		if (_check_if_existing_server_with_same_interface_and_config(*(built_servers.end() - 1), built_servers))
		{
			Logger::log(LOG_DEBUG, "Found preexisting server with same config and interface.");
			delete *(built_servers.end() - 1);
			built_servers.erase(built_servers.end() - 1);
		}
	}
	return (true);
}

// Setup virtual servers
bool	_merge_servers_with_same_network_interface_and_different_configs(std::vector<AServerDispatchSwitch*>& built_servers)//, int *nb_srvs)
{
	std::vector<AServerDispatchSwitch*>::iterator		srv1_it, srv2_it;
	for (srv1_it = built_servers.begin(); srv1_it != (built_servers.end() - 1); ++srv1_it)
	{
		for (srv2_it=(srv1_it + 1); srv2_it != built_servers.end(); ++srv2_it)
		{
			if (*srv1_it && *srv2_it && **srv1_it == **srv2_it)
			{
				std::cout << std::endl << "*----MERGING SERVERS ON SAME NETWORK INTERFACE !!----*" << std::endl;
				std::cout << "Server 1 : " << (*srv1_it)->get_server_name() << std::endl;
				std::cout << "Server 2 : " << (*srv2_it)->get_server_name() << std::endl;
				Logger::log(LOG_DEBUG, "Merging two server configs in one server with same network interface.");
				if (!(*srv1_it)->add_virtual_server(**srv2_it))
					return (Logger::log(LOG_CRITICAL, "Critical error occured while creating server cluster with config file."), false);
				delete *srv2_it;
				built_servers.erase(srv2_it);
				--srv1_it;
				break ;
			}
		}
	}
	return (true);
}

AServerCluster *
ServerFactory::create_cluster_from_cfg(const std::vector<ServerConfig>& cfgs)
{
	AServerCluster			*new_cluster = new AServerCluster;
	std::vector<AServerDispatchSwitch*>	new_servers;
	std::vector<ServerConfig>::const_iterator		cfgs_it;
	std::vector<AServerDispatchSwitch*>::iterator	srvs_it;

	for (cfgs_it = cfgs.begin(); cfgs_it != cfgs.end(); ++cfgs_it)
	{
		if (!ServerFactory::create_servers_from_cfg(*cfgs_it, new_servers))//, &nb_srvs))
		{
			_flush_servers_after_error(new_cluster, &new_servers);
			return (NULL);
		}
	}
	if (!_merge_servers_with_same_network_interface_and_different_configs(new_servers))
	{
		_flush_servers_after_error(new_cluster, &new_servers);
		return (NULL);
	}
	// TODO : implement config file parsing to build config map to pass to this function.
//	std::cerr << "Called unimplemented ServerFactory create_server_from_cfg()." << std::endl;

	for (srvs_it = new_servers.begin(); srvs_it != new_servers.end(); ++srvs_it)
		new_cluster->add_server(*srvs_it);

	return (new_cluster);
}