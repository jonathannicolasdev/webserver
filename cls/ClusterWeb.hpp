/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClusterWeb.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 22:57:13 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/22 20:58:19 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_H
# define SERVER_MANAGER_H

# include "webserv.hpp"
# include "Server.hpp"


class   ClusterWeb: public AServerCluster
{
	private:
		bool	validate_server(IServer *srv);

	public:
		ClusterWeb();
		~ClusterWeb();
		int		start_cluster(void);
		int		add_server(IServer *srv);
		void	stop_cluster(void);
};
/*
ClusterWeb::ClusterWeb(void): _status(CLU_IDLE), _id(this->generate_id())
{
	std::cout << "Cluster constructor" << std::endl;
}

ClusterWeb::~ClusterWeb(void)
{
	std::cout << "Cluster destructor" << std::endl;

	for (int i=0; i < _servers.size(); i++)
	{
		_servers[i]->stop();
		delete _servers[i];
	}	
	std::cout << "Cluster destructor" << std::endl;
}

int	ClusterWeb::start_cluster(void)
{
	std::cout << "Starting Web Cluster" << std::endl;
	_status = CLU_RUNNING;
	return (0);
}

void	ClusterWeb::stop_cluster(void)
{
	std::cout << "Stopping Web Cluster" << std::endl;
	_status = CLU_IDLE;
}

bool	validate_server(IServer *srv)
{
	// Validation conditions to be extended ...
	return (srv.get_status() == SRV_IDLE);
}

int	ClusterWeb::add_server(IServer *srv)
{
	if (this->server_in_cluster(srv) || !this->validate_server(srv))
		return (-1);
	
	_servers.push_back(srv);
	return (0);
}
*/
#endif
