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

bool	_parse_config_ports(const std::string& listen_ports, std::string* listen_addrs, int *ports, int *nb_parsed)
{
	size_t			pos1 = 0, pos2 = 0, pos_mid = 0;
	std::ostringstream	os;
	std::string		sub, addr;
	in_addr_t		net_addr;
	int				num_port;

	*nb_parsed = 0;

	while (pos1 != listen_ports.npos
		&& *nb_parsed < MAX_LISTEN_PORTS
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
		if ((net_addr = inet_addr(addr.c_str())) == INADDR_NONE)
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

		listen_addrs[*nb_parsed] = addr;
		ports[(*nb_parsed)++] = num_port;
		std::cout << "listen addr: " << listen_addrs[*nb_parsed - 1] << std::endl;
		std::cout << "listen port: " << ports[*nb_parsed - 1] << std::endl;
		pos1 = pos2;
		std::cout << "new pos1 : " << pos1 << std::endl;
	}
	if (*nb_parsed == 0)
	{
		addr = "0.0.0.0";
		listen_addrs[0] = "0.0.0.0";
		ports[0] = 80;
		(*nb_parsed)++;
	}
	return (true);
}

bool	ServerFactory::create_servers_from_cfg(const ServerConfig& scfg, IServer **built_servers, int *nb_srvs)
{
	//IServer				*new_server = NULL;
	std::string			listen_str, serverName, rootDir;
	std::string			s;
	std::string			listen_addr[MAX_LISTEN_PORTS];
	int					listen_ports[MAX_LISTEN_PORTS];
	int					nb_ports;

	*nb_srvs = 0;
	std::cout << "Pretend Create server from factory" << std::endl;
	listen_str = scfg.GetListenPort();
	std::cout << "listen str : " << listen_str << std::endl;

	if (!_parse_config_ports(listen_str, listen_addr, listen_ports, &nb_ports))
		return (false);
	// DO SOMETHING 

//	new_server = ServerHTTP(scfg.GetServerName(), );
	for (int i=0; i < nb_ports; ++i, (*nb_srvs)++)
	{
		built_servers[*nb_srvs] = new ServerHTTP(scfg.GetServerName(), scfg.GetRoot(),
												listen_addr[*nb_srvs], listen_ports[*nb_srvs], SRVHTTP_DEFAULT_TIMEOUT, &scfg);
	}
	return (true);
}

AServerCluster *ServerFactory::create_cluster_from_cfg(const std::vector<ServerConfig>& cfg)
{
	AServerCluster	*new_cluster = new AServerCluster;
	IServer			*new_servers[MAX_SERVERS_ON_PORT];
	int				nb_srvs = 0;
	std::vector<ServerConfig>::const_iterator		it;

	for (it = cfg.begin(); it != cfg.end(); ++it)
	{
		if (!ServerFactory::create_servers_from_cfg(*it, new_servers, &nb_srvs))
		{
			std::cerr << "Failed to create server with config:" << std::endl;
			std::cerr << *it << std::endl;
		}
	}
	// TODO : implement config file parsing to build config map to pass to this function.
	std::cerr << "Called unimplemented ServerFactory create_server_from_cfg()." << std::endl;

	// REMOVE ME 
//	delete new_cluster;

//	return (NULL);
	return (new_cluster);
}