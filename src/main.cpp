/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 22:13:49 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/22 22:14:46 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
# include <string>
/*
void	read_available_addr(void)
{
	struct addrinfo	af, *res, *ap;
	int				error;

	std::memset(&af, 0, sizeof(af));
	af.ai_family = AF_INET;
	af.ai_socktype = SOCK_STREAM;
	af.ai_flags = AI_PASSIVE;
	if ((error = getaddrinfo(NULL, "3738", &af, &res)) < 0)
		Logger::log(LOG_DEBUG, std::string("Error while getaddrinfo : ") + gai_strerror(errno));
	ap = res;
	while (ap)
	{
		std::cout << "valid address : " << inet_ntoa(((struct sockaddr_in *)ap->ai_addr)->sin_addr) << std::endl;
		ap = ap->ai_next;
	}
	freeaddrinfo(res);
}
*/
static AServerCluster	*srv_clu;

int	clean_exit(AServerCluster* clu, int exit_status)
{
	if (!clu)
		return (exit_status);
	clu->terminate(true);// Forced termination (true)
//	delete clu;
	return (exit_status);
}

void	sigint_handler(int signum)
{
	UNUSED(signum);
	std::cerr << "Received SIGINT signal. Exiting." << std::endl;
	if (srv_clu)
	{
		srv_clu->terminate(true);
//		delete srv_clu;
	}
	// Don't do this.
	exit(SIGINT);
}

void	sigpipe_handler(int signum)
{
	UNUSED(signum);
	std::cerr << "Received SIGPIPE signal." << std::endl;
	Logger::log(LOG_WARNING, "Received SIGPIPE signal and handling it majestically by disconnecting problematic client.");
	if (srv_clu)
	{
		//srv_clu->terminate(true);
//		delete srv_clu;// implicitally called by ServerCluster destructor
		srv_clu->track_bad_client();
	}
	// Don't do this.
//	exit(SIGINT);
}

int main(int argc, char **argv, char **envp)
{
//	AServerCluster	clu;
	ConfigBuilder	configBuilder;// = ConfigBuilder();
	std::string		config_path;
	std::vector<ServerConfig> serverConfigs;

	(void)envp;

	if (argc < 2)
	{
		std::cerr << "You need to provide a server configuration file to start the server." << std::endl;
		return (EXIT_FAILURE);
//		config_path = "configs/config.txt";
	}
	else
		config_path = argv[1];

	serverConfigs = configBuilder.parseConfigFile(config_path);
	if (serverConfigs.size() == 0)
		return (EXIT_FAILURE);// Error message should have been displayed in configBuilder.parseConfigFile()

	if (!(srv_clu = ServerFactory::create_cluster_from_cfg(serverConfigs)))
		return (EXIT_FAILURE);
//	return (1);

//	std::vector <AServerDispatchSwitch *>	init_servers;
/*
    for(size_t i=0; i < serverConfigs.size(); ++i)
    {
		//int64_t	port = std::stoi(serverConfigs[i].GetListenPort());
		int64_t	port = atoi(serverConfigs[i].GetListenPort().c_str());
		if (port < 1 || port > 65535)
			return (Logger::log(LOG_ERROR, "Listening port invalid"));
		std::cout << "listening port : " << port << std::endl;
        serverConfigs[i].print();
//		init_servers.push_back(
		if (clu.add_server(new ServerHTTP(serverConfigs[i].GetRoot(), serverConfigs[i].GetServerName(),
				serverConfigs[i].GetHostIp(), port)) < 0)
			return (Logger::log(LOG_ERROR, "Adding Server Failed."));
    }
*/
//	ServerHTTP	*srv = new ServerHTTP("www/", "Jimbo Jones", "", 3738);
//	ServerHTTP	*srv2 = new ServerHTTP("www/", "Jimby Jims", "", 80);

//	read_available_addr();
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, sigpipe_handler);

//	if (clu.add_server(srv) < 0 || clu.add_server(srv2) < 0)
//		return (clean_exit(clu));
	
	if (srv_clu->bind() < 0)
		return (clean_exit(srv_clu, EXIT_FAILURE));

	if (srv_clu->start() < 0)
		return (clean_exit(srv_clu, EXIT_FAILURE));

/*
	if (srv.bind_server() < 0)// || srv.)
		return (1);
	
	std::cout << "Try Starting Server" << std::endl;
	if (srv.start() < 0)
		return (2);
*/
	std::cout << "Exiting peacefully" << std::endl;
	return (clean_exit(srv_clu, EXIT_SUCCESS));
	//return (cle);
}
