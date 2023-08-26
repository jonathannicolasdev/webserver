/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServerCluster.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 21:12:22 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/22 21:13:09 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AServerCluster.hpp"

int	log_bind_error(std::string &srv_name, int port)
{
	std::ostringstream	log_msg;

	log_msg << "Server named " << srv_name;
	log_msg << "on port " << port;
	log_msg << " failed to bind itself. All servers in a cluster must be able to bind, otherwise no server owned by this cluster can be bound.";

	return (Logger::log(LOG_WARNING, log_msg.str()));
}

int	log_start_error(std::string &srv_name, int port)
{
	std::ostringstream	log_msg;

	log_msg << "Server named " << srv_name;
	log_msg << "on port " << port;
	log_msg << " failed to start itself. All servers in a cluster must be able to start, otherwise no server owned by this cluster are allowed to start. Stopping Cluster. Call ServerCluster::bind again to reattempt start.";

	return (Logger::log(LOG_WARNING, log_msg.str()));
}

int	AServerCluster::_counter = 0;

int	AServerCluster::generate_id(void)
{
	return (this->_counter++);
}

AServerCluster::AServerCluster(void): __EventListener(), _id(generate_id()),
	_status(CLU_UNBOUND), _request_stop(false), _timeout(-1), _currently_serving(NULL)
{
	std::memset(&this->_new_event, 0, sizeof(this->_new_event));
	std::memset(this->_changes, 0, sizeof(this->_changes));
	this->_pollfd = 0;
}

AServerCluster::~AServerCluster(void)
{
	this->terminate(true);
}

bool	AServerCluster::contains(AServerDispatchSwitch *srv) const
{
	std::map<int, AServerDispatchSwitch *>::const_iterator	it;

	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
		if (it->second == srv)
			return (true);
	return (false);
}

AServerDispatchSwitch*
AServerCluster::find_owner(int client_fd) const
{
	std::map<int, AServerDispatchSwitch *>::const_iterator	it;

	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
		if (it->second->is_serving(client_fd))
			return (it->second);
	return (NULL);
}

AServerDispatchSwitch*
AServerCluster::find_server(int socket_fd) const
{
	std::map<int, AServerDispatchSwitch *>::const_iterator	it;

	it = this->_servers.find(socket_fd);
	if (it == this->_servers.end())
		return (NULL);
	else
		return (it->second);
}

int
AServerCluster::get_nb_managed(void) const { return (this->_server_set.size());}

bool
AServerCluster::validate_server(AServerDispatchSwitch *srv) const
{
	t_srv_state				*srv_state;
	std::ostringstream		port_str;
	std::map<int, AServerDispatchSwitch*>::const_iterator	it;

	if (!srv)
		return (Logger::log(LOG_WARNING, "Passed nullptr to AServerCluster::add_server()"), false);
	srv_state = srv->get_srv_state();
	if (srv_state->status_code != SRV_UNBOUND)
		return (Logger::log(LOG_WARNING, std::string("Cannot add to a Cluster a server with status other than SRV_UNBOUND. This server status : ")
			+ srv_state->status), false);

	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		if (srv == it->second)
			return (Logger::log(LOG_WARNING, std::string("Trying to add same server twice in ServerCluster. Server name : ")
				+ srv_state->server_name), false);
		port_str << srv_state->port;
		if (it->second->get_port() == srv_state->port)
			return (Logger::log(LOG_WARNING, std::string("ServerCluster cannot have two servers binding to the same port. Conflicting port : ")
				+ port_str.str()), false);
	}
	return (true);
}

int
AServerCluster::add_server(AServerDispatchSwitch *srv)
{
	std::ostringstream	id_str;
	int					timeout;

	if (!this->validate_server(srv))
		return (-1);
	Logger::log(LOG_DEBUG, std::string("Adding new server to cluster id ") + id_str.str() + " Server info : ");
	id_str << this->_id;
	std::cout << *srv << std::endl;
	this->_server_set.insert(srv);
	timeout = srv->get_timeout();
	if (this->_timeout == -1)
		this->_timeout = timeout;
	else if (timeout >= 0 && timeout < this->_timeout)
		this->_timeout = timeout;

	return (0);
}

int
AServerCluster::bind(void)
{
	std::set<AServerDispatchSwitch*>::const_iterator	it;
	t_srv_state		*srv_state;

	if (this->get_nb_managed() == 0)
		return (Logger::log(LOG_DEBUG, "Cannot bind a cluster with no servers under managment. Add at least one server to proceed."));
	
	if (this->_status != CLU_UNBOUND)
		return (Logger::log(LOG_WARNING, "Cluster must be at CLU_UNBOUND stage to attempt to bind its servers. Call ServerCluster::stop() or reboot() if you think something went wrong."));
	
	if (this->init_poll() < 0)
		return (this->stop(), -1);
	for (it = this->_server_set.begin(); it != this->_server_set.end(); ++it)
	{
		if ((*it)->bind_server() < 0)
		{
			srv_state = (*it)->get_srv_state();

			log_bind_error(srv_state->server_name, srv_state->port);
			this->stop();
			return (-1);
		}
		this->_servers[(*it)->get_sockfd()] = *it;
	}
	this->_status = CLU_IDLE;
	return (0);
}

void
AServerCluster::do_maintenance(void)
{
	std::map<int, AServerDispatchSwitch*>::const_iterator	it;
	int		nb_disconn;
	int		timeout_clients[MAX_CONCUR_POLL];
	time_t	curr_time = std::time(NULL);

	if ((curr_time - this->_last_maintenance_time) * 1000 < this->_timeout)
		return ;
	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		nb_disconn = it->second->do_maintenance(timeout_clients, MAX_CONCUR_POLL);
		for (int i=0; i < nb_disconn; ++i)
			this->unpoll_socket(timeout_clients[i]);
	}
	this->_last_maintenance_time = std::time(NULL);
}

int 
AServerCluster::__cluster_mainloop(void)
{
	AServerDispatchSwitch	*srv;
	int		nb_events;
	int		eventfd, sockfd, clientfd;
	int		disconn_clients[MAX_CONCUR_POLL];
	int		nb_disconn;
	int		i;

	this->_status = CLU_RUNNING;
	while (!this->_request_stop)
	{
		nb_events = this->poll_wait(this->_timeout);

		// Called here but only does maintenance if > timeout time has past since
		// last maintenance or start of cluster.
		if (nb_events <= 0)
		{
			// do basic maintenance of cluster and its servers.
			this->do_maintenance();
			continue ;
		}

		for (i = 0; i < nb_events; ++i)
		{
			eventfd = this->get_eventfd(i);

			// Looks for the sockfd (eventfd) in the cluster to check weither this fd is a server socket
			// and that it is the owner. Return Server pointer if so, NULL otherwise.
			srv = this->find_server(eventfd);
			if (srv)
			{
				// The event was triggered by a socket and we must accept() a new connection from it.
				sockfd = eventfd;
				nb_disconn = srv->connect(disconn_clients, MAX_CONCUR_POLL, &clientfd);
				fcntl(clientfd, F_SETFL, O_NONBLOCK);
				if (nb_disconn < 0)
				{
					// Error happened while connecting, but show must go on.
					continue ;
				}
				if (nb_disconn)
					this->unpoll_socket_array(disconn_clients, nb_disconn);
				this->poll_new_socket(clientfd);
			}
			else if ((srv = this->find_owner(eventfd)))
			{
				// The event was triggered by clientfd and we must serve the requested content.
				_currently_serving = srv;
				clientfd = eventfd;
				if (srv->serve_request(clientfd) < 0)
				{
					this->unpoll_socket(clientfd);
					srv->disconnect(clientfd, true);
				}
				_currently_serving = NULL;
				srv->disconnect(clientfd, false);
			}
			else
				return (Logger::log(LOG_CRITICAL, "POLLING MECH RECEIVED EVENT FROM UNIDENTIFIED FD. THE TRUTH IS OUT THERE !!"));
		}
	}
	return (0);
}

int
AServerCluster::start(void)
{
	std::map<int, AServerDispatchSwitch*>::iterator	it;
	t_srv_state							*srv_state;
	int									sockfd;

	if (this->_status != CLU_IDLE)
		return (Logger::log(LOG_DEBUG, "Cluster must be at CLU_IDLE stage, with all servers bound, to attempt to start. Call ServerCluster::bind() first before attempting to start cluster."));
	
	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		if ((sockfd = it->second->start()) < 0)
		{
			srv_state = it->second->get_srv_state();
			log_start_error(srv_state->server_name, srv_state->port);
			this->stop();
			return (-1);
		}
		this->poll_new_socket(sockfd);
	}

	/// Start main loop
	this->_start_time = std::time(NULL);
	this->_last_maintenance_time = this->_start_time;
	std::cout << "Listening to requests ..." << std::endl;
	return (this->__cluster_mainloop());
}

void
AServerCluster::track_bad_client(void)
{
	AServerDispatchSwitch*	srv;
	
	srv = _currently_serving;
	if (!srv)
		return;
	srv->disconnect_current_client();
}

void
AServerCluster::stop(void)
{
	std::map<int, AServerDispatchSwitch*>::iterator	it;

	this->_request_stop = true;	
	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		this->unpoll_socket(it->first);
		it->second->stop();
	}
	this->_status = CLU_UNBOUND;
}

int
AServerCluster::terminate(bool force)
{
	std::map<int, AServerDispatchSwitch*>::iterator	it;

	if (!force && (this->_status == CLU_RUNNING))
		return (Logger::log(LOG_WARNING, "Cannot terminate a running cluster. To force a cluster to terminate, call terminate(true) instead."));

	this->stop();
	this->_servers.clear();
	this->close_poll();

	this->_status = CLU_UNBOUND;
	return (0);
}

int
AServerCluster::reboot(void)
{
	this->stop();
	if (this->bind() < 0
		|| this->start() < 0)
		return (Logger::log(LOG_WARNING, "Cluster failed to reboot"));
	Logger::log(LOG_DEBUG, "Cluster successfully rebooted");
	return (0);
}
