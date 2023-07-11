/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServerCluster.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 20:40:13 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/22 22:30:31 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef A_SERVER_CLUSTER_HPP
# define A_SERVER_CLUSTER_HPP

//# include "webserv.hpp"
# include <vector>
# include "Server.hpp"

enum e_cluster_status
{
	CLU_IDLE,
	CLU_RUNNING,
	CLU_BROKEN
};

// AServerCluster (Abstract Server Cluster):
//	Is a generic group of generic server objects. Servers can be added to
//	this set of servers to form a group of servers working on a common task
//	either in parallel on the same task or loosely coupled doing related tasks.
//	When a server is added to a cluster, IT IS OWNED BY THE CLUSTER. Therefor,
//	the cluster is solely responsible for the server's life cycle after it joins the
//	cluster. This means that if the cluster is stopped, all servers should be
//	stopped and deleted BY THE CLUSTER.

//	Example case: A Web Cluster manages 1 HTTP Server, 2 FTP Server and
//	1 SSH Server and communicates to a Database Cluster with
//	3 database servers and 1 SSH server. A wacky world we live in.

//	A Database cluster and a Web Cluster would be 2 concrete extensions to
//	the Abstract Server Cluster class.

class	AServerCluster
{
	protected:
		static int	_counter;

		int			_id;
		int			_status;
		int			generate_id(void);

		int			_pollfd;
		
		std::vector<IServer *>	_servers;//	All servers in cluster
		

		virtual bool	validate_server(IServer *srv) const = 0;//	Called when adding new server
									//	Specific to cluster type

	public:
		bool			contains(IServer *srv) const;
		IServer			*find_owner(int client_fd) const;

		virtual int		start_cluster(void) = 0;
		virtual int		add_server(IServer *srv) = 0;
		virtual void	stop_cluster(void) = 0;
};
/*
int	AServerCluster::_counter = 0;

int	AServerCluster::generate_id(void)
{
	return (this->_counter++);
}

bool	AServerCluster::server_in_cluster(IServer *srv)
{
	return (std::find(_servers.begin(), _servers.end(), srv) != _servers.end())
}
*/
# endif
