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

//std::map<enum e_srv_type, t_srv_cfg>	ServerFactory::default_cfgs = {
//int	ServerFactory::default_cfgs = {
//	{SRVF_HTTP, {(uint16_t)PORT_HTTP, "./"}}
//}

// Creates and builds a new instance of a Server of the type mentioned as parameter
// 
/*
IServer *ServerFactory::create_server(enum e_srv_type_srv srv_type, uint16_t port,
	const std::string& rootdir)
{
	std::cout << "Creating server from ServerFactory. type " << srv_type << std::endl;
	switch (srv_type) {
		case SRVF_HTTP:
			return (new ServerHTTP(
				(!port) ? (PORT_HTTP):(port),
				(!rootdir) ? ("./"):(rootdir)));

//  Add more cases with their 
		default:
			std::cerr << "Server factory failed to create server of type code "
				<< srv_type << " because : Not implemented." << std::endl;
				return (NULL);
	}
}
*/

bool	_parse_config_ports(const std::string& listen_ports,
	std::vector<std::string>& listen_addrs, std::vector<int>& ports)
{
	size_t			pos1 = 0, pos2 = 0, pos_mid = 0;
	std::ostringstream	os;
	std::string		sub, addr;
//	in_addr_t		net_addr;
	int				num_port;

//	*nb_parsed = 0;

	while (pos1 != listen_ports.npos
//		&& *nb_parsed < MAX_LISTEN_PORTS
		&& (pos1 = listen_ports.find_first_not_of(" ,\t\n", pos1)) != listen_ports.npos)
	{
		std::cout << "ENTER LISTEN PORT WHILE LOOP." << std::endl;
		pos2 = listen_ports.find_first_of(" ,\t\n", pos1);
		if (pos2 == listen_ports.npos)
			sub = listen_ports.substr(pos1);
		else
			sub = listen_ports.substr(pos1, pos2 - pos1);

		std::cout << "STAGE 1 COMPLET : " << sub << std::endl;

		if ((pos_mid = sub.find(':')) == sub.npos)
		{
			sub = "0.0.0.0:" + sub;
			pos_mid = sub.find(':');
		}
		std::cout << "STAGE 2 COMPLET : " << sub << ", pos1, mid: " << pos1 << ", " << pos_mid << std::endl;
		
		addr = sub.substr(0, pos_mid);
		std::cout << "STAGE 2.5 COMPLET" << std::endl;
		if (inet_addr(addr.c_str()) == INADDR_NONE)
		{
			Logger::log(LOG_ERROR, std::string("Configuration file listen address ") + addr + " is not a valid network ip address.");
			return (false);
		}
		std::cout << "STAGE 3 COMPLET" << std::endl;
		sub = sub.substr(pos_mid + 1, pos2 - pos_mid - 1);
		num_port = atoi(sub.c_str());
		if (num_port < 1 || 65535 < num_port || !is_all_digits(sub))
		{
			os << num_port;
			std::cout << sub.length() << " | " << !is_all_digits(sub) << std::endl;
			Logger::log(LOG_ERROR, std::string("Configuration file listen port ") + os.str() + " is not a valid port number.");
			return (false);
		}

//		listen_addrs[*nb_parsed] = addr;
//		ports[(*nb_parsed)++] = num_port;
		listen_addrs.push_back(addr);
		ports.push_back(num_port);
		std::cout << "listen addr: " << *(listen_addrs.end() - 1) << std::endl;
		std::cout << "listen port: " << *(ports.end() - 1) << std::endl;
		pos1 = pos2;
		std::cout << "new pos1 : " << pos1 << std::endl;
	}
//	if (*nb_parsed == 0)
	if (ports.empty())
	{
		addr = "0.0.0.0";
		listen_addrs.push_back("0.0.0.0");
		ports.push_back(80);
//		(*nb_parsed)++;
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
	//for (int i=0; i < nb_srvs; ++i)
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
	//IServer				*new_server = NULL;
	std::string			listen_str, serverName, rootDir;
//	std::string			s;
//	std::string			listen_addr[MAX_LISTEN_PORTS];
//	int					listen_ports[MAX_LISTEN_PORTS];
	std::vector<std::string>	listen_addr;
	std::vector<int>			listen_ports;
//	int					nb_ports;
//	std::vector<AServerDispatchSwitch*>::iterator		srv_it = built_servers.begin();
	std::vector<std::string>::iterator	addr_it;// = listen_addr.begin();
	std::vector<int>::iterator			port_it;// = listen_ports.begin();

//	*nb_srvs = 0;
	std::cout << "Pretend Create server from factory" << std::endl;
	listen_str = scfg.GetListenPort();
	std::cout << "listen str : " << listen_str << std::endl;

	if (!_parse_config_ports(listen_str, listen_addr, listen_ports))//, &nb_ports))
		return (false);
	// DO SOMETHING 
	std::cout << "Finished parsing ports" << std::endl;
//	new_server = ServerHTTP(scfg.GetServerName(), );
//	for (int i=0; i < nb_ports; ++i, ++(*nb_srvs))
 	addr_it = listen_addr.begin();
 	port_it = listen_ports.begin();
//	std::cout << "addr: " << *addr_it << std::endl;
	for (; port_it != listen_ports.end(); ++addr_it, ++port_it)//, ++(*nb_srvs))
	{
		std::cout << "Building server" << std::endl;
		std::cout << " - server name :	" << scfg.GetServerName() << std::endl;
		std::cout << " - root dir :		" << scfg.GetRoot() << std::endl;
		std::cout << " - ip addr  :		" << *addr_it << std::endl;
		std::cout << " - port	 :		" << *port_it << std::endl;
//		built_servers[*nb_srvs] = new ServerHTTP(scfg.GetServerName(), scfg.GetRoot(),
		built_servers.push_back(new ServerHTTP(scfg.GetServerName(), scfg.GetRoot(),
												*addr_it, *port_it, SRVHTTP_DEFAULT_TIMEOUT, &scfg));
		//if (_check_if_existing_server_with_same_interface_and_config(built_servers[*nb_srvs], built_servers, *nb_srvs))
		if (_check_if_existing_server_with_same_interface_and_config(*(built_servers.end() - 1), built_servers))
		{
			Logger::log(LOG_DEBUG, "Found preexisting server with same config and interface.");
			delete *(built_servers.end() - 1);
			built_servers.erase(built_servers.end() - 1);
//			return (_flush_servers_after_error(built_servers));
		}
	}
	return (true);
}

// Setup virtual servers
bool	_merge_servers_with_same_network_interface_and_different_configs(std::vector<AServerDispatchSwitch*>& built_servers)//, int *nb_srvs)
{
	std::vector<AServerDispatchSwitch*>::iterator		srv1_it, srv2_it;
//	IServer		*srv1, *srv2;

	//for (int i=0; i < (*nb_srvs - 1); ++i)
	for (srv1_it = built_servers.begin(); srv1_it != (built_servers.end() - 1); ++srv1_it)
	{
//		srv1 = built_servers[i];
		for (srv2_it=(srv1_it + 1); srv2_it != built_servers.end(); ++srv2_it)
		{
//			srv2 = built_servers[j];
			if (**srv1_it == **srv2_it)
			{
				std::cout << std::endl << "*----MERGING SERVERS ON SAME NETWORK INTERFACE !!----*" << std::endl;
				std::cout << "Server 1 : " << (*srv1_it)->get_server_name() << std::endl;
				std::cout << "Server 2 : " << (*srv2_it)->get_server_name() << std::endl;
				Logger::log(LOG_DEBUG, "Merging two server configs in one server with same network interface.");
				if (!(*srv1_it)->add_virtual_server(**srv2_it))
					return (Logger::log(LOG_CRITICAL, "Critical error occured while creating server cluster with config file."));
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
//	IServer					*new_servers[MAX_SERVERS_ON_PORT];
	std::vector<AServerDispatchSwitch*>	new_servers;
//	int						nb_srvs = 0;
	std::vector<ServerConfig>::const_iterator		cfgs_it;
	std::vector<AServerDispatchSwitch*>::iterator	srvs_it;

	for (cfgs_it = cfgs.begin(); cfgs_it != cfgs.end(); ++cfgs_it)
	{
		if (!ServerFactory::create_servers_from_cfg(*cfgs_it, new_servers))//, &nb_srvs))
		{
//			std::cerr << "Failed to create server with config:" << std::endl;
//			std::cerr << *it << std::endl;
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

	// REMOVE ME 
//	delete new_cluster;
	for (srvs_it = new_servers.begin(); srvs_it != new_servers.end(); ++srvs_it)
		new_cluster->add_server(*srvs_it);

//	return (NULL);
	return (new_cluster);
}