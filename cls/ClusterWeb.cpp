/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClusterWeb.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 21:14:11 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/22 21:14:33 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClusterWeb.hpp"

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
