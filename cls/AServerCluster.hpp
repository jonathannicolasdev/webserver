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
//# include <vector>
//# include <set>
# include <map>
# include "IServer.hpp"

# define MAX_POLL_EVENTS 64

enum e_cluster_status
{
	CLU_UNBOUND,
	CLU_IDLE,
	CLU_RUNNING,
	CLU_BROKEN
};

// This class is inherited by AServerCluster and implements its event polling mechanisms.
// It is separate from the AServerCluster class mainly because the implementation
// for these functionalities is highly depend on OS.
class __EventListener
{
	protected:
		int				_pollfd;
#ifdef __APPLE__
		struct kevent	_events[MAX_POLL_EVENTS];
#elif __linux__
		struct epoll_event	_events[MAX_POLL_EVENTS];
#endif

		// The word poll is used here for simplicity but 
		// will be either epoll of kqueue depending on OS.
		virtual int		init_poll(void);
		virtual int		close_poll(void);
		virtual int		poll_new_socket(int sockfd);
		virtual int		unpoll_socket(int sockfd);
		virtual int		poll_new_client(int clientfd);
		virtual int		unpoll_client(int clientfd);
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

class	AServerCluster:	public __EventListener
{
	protected:
		static int	_counter;

		int			_id;
		int			_status;
		int			generate_id(void);

		
//		std::vector<IServer *>	_servers;//	All servers in cluster
		//std::set<IServer *>	_servers;//	All servers in cluster
		std::map<int, IServer *>	_servers;//	All servers in cluster. key == sockfd, value == server instance.
		
		virtual bool	validate_server(IServer *srv) const = 0;//	Called when adding new server
																//	Specific to cluster type

		IServer			*find_owner(int client_fd) const;/// Finds the server to which a client fd belongs to if any.
		IServer			*find_server(int socket_fd) const;// Get server from socket fd. Used when polling server socket
															//	for new connections. Poll (or equiv.) will return the socket fd
															//	and passing it to this function retreives the server inst it belongs to.

	public:
		int				get_nb_managed(void) const;
		bool			contains(IServer *srv) const;

		virtual int		bind(void);// = 0;
		virtual int		start(void);// = 0;
		virtual int		add_server(IServer *srv);// = 0;
		virtual void	stop(void);// = 0;// stops all its active servers
		virtual void	reboot(void);// = 0;// stops all its active servers and restarts them.
		virtual void	terminate(bool force);// = 0;// stops all active servers and deletes them from the cluster.

		// Potentialy add multiple overloads for stopping / rebooting / starting ... by Server type.
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
