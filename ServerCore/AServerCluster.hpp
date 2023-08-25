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

# include <fcntl.h>
# include <set>
# include <map>
# include "AServerDispatchSwitch.hpp"
# include "webserv_utils.hpp"

# ifdef __APPLE__
#  include <sys/event.h>
# elif __linux__
#  include <sys/epoll.h>
# endif

# define MAX_CONCUR_POLL 1024

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
		int				nb_watched;
#ifdef __APPLE__
		struct kevent	_new_event;
		struct kevent	_changes[MAX_CONCUR_POLL];
#elif __linux__
		struct epoll_event	_new_event;
		struct epoll_event	_changes[MAX_CONCUR_POLL];
#endif

		// The word poll is used here for simplicity but 
		// will be either epoll of kqueue depending on OS.
		virtual int		init_poll(void);
		virtual int		close_poll(void);
		virtual int		poll_new_socket(int sockfd);
		virtual int		unpoll_socket(int sockfd);
		virtual void	unpoll_socket_array(int *sockfds, int nb_sockets);

		virtual int		poll_wait(int timeout);

		virtual int		get_eventfd(int event_idx) const;
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

class	AServerCluster:	public __EventListener, public __BaseSocketOwner
{
	protected:
		static int	_counter;

		int			_id;
		int			_status;
		int			generate_id(void);
		bool		_request_stop;
		int			_timeout;// minimum none 0 timeout in member servers.
		time_t		_start_time;
		time_t		_last_maintenance_time;
		AServerDispatchSwitch*		_currently_serving;// The server currently serving a client.
		
		std::set<AServerDispatchSwitch *>	_server_set;//	All servers in cluster. key == sockfd, value == server instance.
		std::map<int, AServerDispatchSwitch *>	_servers;//	All servers in cluster. key == sockfd, value == server instance.
		
		virtual bool	validate_server(AServerDispatchSwitch *srv) const;//	Called when adding new server
																				//	Specific to cluster type

		AServerDispatchSwitch	*find_owner(int client_fd) const;/// Finds the server to which a client fd belongs to if any.
		AServerDispatchSwitch	*find_server(int socket_fd) const;// Get server from socket fd. Used when polling server socket
																//	for new connections. Poll (or equiv.) will return the socket fd
																//	and passing it to this function retreives the server inst it belongs to.

		void			do_maintenance(void);
		int				__cluster_mainloop(void);

	public:

		AServerCluster(void);
		virtual ~AServerCluster(void);

		int				get_nb_managed(void) const;
		bool			contains(AServerDispatchSwitch *srv) const;
		
		virtual int		add_server(AServerDispatchSwitch *srv);// = 0;

		virtual int		bind(void);
		virtual int		start(void);
		virtual void	stop(void);// Stops all its active servers
		virtual int		reboot(void);// Stops all its active servers and restarts them.
		virtual int		terminate(bool force);// Stops all active servers and deletes them from the cluster.
		virtual void	track_bad_client(void);// Disconnect the client currently beeing served, on the server currently servicing its request.
};

# endif
