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

	log_msg << "Server named " << srv_state->server_name;
	log_msg << "on port " << srv_state->port;
	log_msg << " failed to bind itself. All servers in a cluster must be able to bind, otherwise no server owned by this cluster can be bound.");

	return (Logger::log(LOG_WARNING, log_msg.str()));
}

int	log_start_error(std::string &srv_name, int port)
{
	std::ostringstream	log_msg;

	log_msg << "Server named " << srv_state->server_name;
	log_msg << "on port " << srv_state->port;
	log_msg << " failed to start itself. All servers in a cluster must be able to start, otherwise no server owned by this cluster are allowed to start. Stopping Cluster. Call ServerCluster::bind again to reattempt start.");

	return (Logger::log(LOG_WARNING, log_msg.str()));
}

int	AServerCluster::_counter = 0;

int	AServerCluster::generate_id(void)
{
	return (this->_counter++);
}

AServerCluster::AServerCluster(void): _id(generate_id()), _status(CLU_IDLE), _pollfd(0)
{
	std::cout << "AServerCluster constructor" << std::endl;
}

AServerCluster::~AServerCluster(void)
{
	std::cout << "AServerCluster destructor" << std::endl;
	this->terminate();
}

bool	AServerCluster::contains(IServer *srv) const
{
	return (std::find(_servers.begin(), _servers.end(), srv) != _servers.end())
}

IServer*
AServerCluster::find_owner(int client_fd) const
{
	//std::vector<IServer *>::iterator	it;
	std::map<int, IServer *>::iterator	it;

	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
		//if ((*it)->is_serving(client_fd))
		if (it->second->is_serving(client_fd))
			return (it);
	return (nullptr);
}

IServer*
AServerCluster::find_server(int socket_fd) const
{
	std::map<int, IServer *>::iterator	it;

	it = this->_servers.find(socket_fd);
	if (it == this->_servers.end())
		return (nullptr);
	else
		return (it->second);
//	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
//		if ((*it)->is_serving(client_fd))
//			return (it);
}

int
AServerCluster::get_nb_managed(void) const
{
	return (this->_servers.size());
}

bool
AServerCluster::validate_server(IServer *srv) const
{
	t_srv_state	*srv_state;
	//std::vector<IServer*>::iterator it;
	std::map<int, IServer*>::iterator it;

	if (!srv)
		return (Logger::log(LOG_WARNING, "Passed nullptr to AServerCluster::add_server()"), false);
	srv_state = srv->get_srv_state();
	if (srv_state->status_code != SRV_UNBOUND)
		return (Logger::log(LOG_WARNING, std::string("Cannot add to a Cluster a server with status other than SRV_UNBOUND. This server status : ")
			+ srv_state->status), false);
	
	for (it = this->_servers.begin() ; it != this->_servers.end(); ++it)
	{
		//if (srv == (*it))
		if (srv == it->second)
			return (Logger::log(LOG_WARNING, std::string("Trying to add same server twice in ServerCluster. Server name : ")
				+ srv_state->server_name), false);
		//if ((*it)->get_port() == srv_state->port)
		if (it->second->get_port() == srv_state->port)
			return (Logger::log(LOG_WARNING, std::string("ServerCluster cannot have two servers binding to the same port. Conflicting port : ")
				+ srv_state.port), false);
	}
	return (true);
}

int
AServerCluster::add_server(IServer *srv)
{
	if (!this->validate_server(srv))
		return (-1);
	Logger::log(LOG_DEBUG, std::string("Adding new server to cluster id ") + this->_id + " Server info : ");
	std::cout << srv << std::endl;
//	this->_servers.push_back(srv);
	this->_servers[srv->get_socket()] = srv;
	return (0);
}

int
AServerCluster::bind(void)
{
	//std::vector<IServer*>::iterator	it;
	std::map<int, IServer*>::iterator	it;
	t_srv_state						*srv_state;

	if (this->get_nb_managed() == 0)
		return (Logger::log(LOG_DEBUG, "Cannot bind a cluster with no servers under managment. Add at least one server to proceed."));
	
	if (this->_status != CLU_UNBOUND)
		return (Logger::log(LOG_WARNING, "Cluster must be at CLU_UNBOUND stage to attempt to bind its servers. Call ServerCluster::stop() or reboot() if you think something went wrong.");
	
	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		//if ((*it)->bind_server() < 0)
		if (it->second->bind_server() < 0)
		{
			//srv_state = (*it)->get_srv_state();
			srv_state = it->second->get_srv_state();

			log_bind_error(srv_state->server_name, srv_state->port);
			this->stop();
			return (-1);
		}
	}
	if (this->init_poll() < 0)
		return (this->stop(), -1);
	this->_status = CLU_IDLE;
	return (0);
}


int
AServerCluster::start(void)
{
//	std::vector<IServer*>::iterator	it;
	std::map<int, IServer*>::iterator	it;
	t_srv_state							*srv_state;
	int									sockfd;

	if (this->_status != CLU_IDLE)
		return (Logger::log(LOG_DEBUG, "Cluster must be at CLU_IDLE stage, with all servers bound, to attempt to start. Call ServerCluster::bind() first before attempting to start cluster.");
	
	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		//if ((sockfd = (*it)->start()) < 0)
		if ((sockfd = it->second->start()) < 0)
		{
			//srv_state = (*it)->get_srv_state();
			srv_state = it->second->get_srv_state();
			log_start_error(srv_state->server_name, srv_state->port);
			this->stop();
			return (-1);
		}
		this->poll_new_socket(sockfd);
	}
	this->_status = CLU_RUNNING;
	return (0);
}

void
AServerCluster::stop(void)
{
	//std::vector<IServer*>::iterator	it;
	std::map<int, IServer*>::iterator	it;
	
	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		it->second->stop();
		//(*it)->stop();
		this->unpoll_socket(it->first);
	}
	this->_status = CLU_UNBOUND;
}

int
AServerCluster::terminate(bool force)
{
//	IServer*	srv;
	std::map<int, IServer*>::iterator	it;

	if (!force && (this->_status == CLU_RUNNING))
		return (Logger::log(LOG_WARNING, "Cannot terminate a running cluster. To force a cluster to terminate, call terminate(true) instead."));

	this->stop();
	this->_servers.clear();
	this->close_poll();
	/*
	while (!this->_servers.empty())
	{
		//srv = this->_servers.front();
		it = this->_servers.begin();
		it->second->stop();
		this->servers.erase(it);
//		try:
//		srv->stop();
//		catch (e)
//			Logger::log(LOG_ERROR, std::string("server on port ") + srv->get_port() + " on Cluster " + this->_id + " failed to stop properly.");

//		delete srv;
//		this->servers.erase(0);
	}
	*/
	this->_status = CLU_UNBOUND;
	return (0);
}

int
AServerCluster::reboot(void)
{
//	std::vector<IServer*>::iterator	it;
	
	if (this->stop() < 0
		|| this->bind() < 0
		|| this->start() < 0)
		return (Logger::log(LOG_WARNING, "Cluster failed to reboot");
	Logger::log(LOG_DEBUG, "Cluster successfully rebooted");
	return (0);
}
