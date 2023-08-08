/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFactory.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 21:06:30 by marvin            #+#    #+#             */
/*   Updated: 2023/07/03 21:06:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_FACTORY_HPP
# define SERVER_FACTORY_HPP

# include "IServer.hpp"
# include "AServerCluster.hpp"
# include "ServerHTTP.hpp"
# include "ServerConfig.hpp"
//# include <netdb.h>

#define MAX_LISTEN_PORTS 	32
#define MAX_SERVERS_ON_PORT	32

// Implement more as needed
enum    e_srv_type
{
	SRVF_HTTP
//	SRVF_HTTPS,
//	SRVF_FTP,
//	SRVF_SSH,
//	SRVF_DB,
//	SRVF_LOG
};

// Add parameter as needed 
typedef struct s_default_srv_config
{
	uint16_t		port;
	std::string		rootdir;
}	t_srv_cfg;

class ServerFactory
{
//	private:
//		static const enum e_srv_type 						*__valid_srv_types;
//		static const std::map<enum e_srv_type, t_srv_cfg>	default_cfgs;

	public:
//		static IServer  *create_server(enum e_srv_type srv_type, uint16_t port, const std::string &rootdir);
		static bool	create_servers_from_cfg(const ServerConfig& scfg, IServer **built_servers, int *nb_srvs);
		static AServerCluster  *create_cluster_from_cfg(const std::vector<ServerConfig>& cfg);
};


#endif