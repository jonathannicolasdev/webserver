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
#include <string>

static AServerCluster	*srv_clu;

int	clean_exit(AServerCluster* clu, int exit_status)
{
	if (!clu)
		return (exit_status);
	clu->terminate(true);
	return (exit_status);
}

void	sigint_handler(int signum)
{
	UNUSED(signum);
	std::cerr << "Received SIGINT signal. Exiting." << std::endl;
	if (srv_clu)
		srv_clu->terminate(true);
}

void	sigpipe_handler(int signum)
{
	UNUSED(signum);
	std::cerr << "Received SIGPIPE signal." << std::endl;
	Logger::log(LOG_WARNING, "Received SIGPIPE signal and handling it majestically by disconnecting problematic client.");
	if (srv_clu)
		srv_clu->track_bad_client();
}

int main(int argc, char **argv, char **envp)
{
	ConfigBuilder	configBuilder;
	std::string		config_path;
	std::vector<ServerConfig> serverConfigs;

	(void)envp;

	if (argc < 2)
	{
		std::cerr << "You need to provide a server configuration file to start the server." << std::endl;
		return (EXIT_FAILURE);
	}
	else
		config_path = argv[1];

	serverConfigs = configBuilder.parseConfigFile(config_path);
	if (serverConfigs.size() == 0)
		return (EXIT_FAILURE);

	// The variable srv_clu is a global variable declared at the top of this file. Is a pointer to a AServerCluster instance.
	// Is global to be accessible by signal handlers.
	if (!(srv_clu = ServerFactory::create_cluster_from_cfg(serverConfigs)))
		return (EXIT_FAILURE);

	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, sigpipe_handler);

	Logger::init(LOGFILE_DEFAULT);
	if (srv_clu->bind() < 0)
		return (clean_exit(srv_clu, EXIT_FAILURE));

	if (srv_clu->start() < 0)
		return (clean_exit(srv_clu, EXIT_FAILURE));

	Logger::close();
	std::cout << "Exiting peacefully" << std::endl;
	return (clean_exit(srv_clu, EXIT_SUCCESS));
}
