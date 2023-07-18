/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServerDispatchSwitch.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 22:11:33 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/11 19:04:46 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AServerDispatchSwitch.hpp"

AServerDispatchSwitch::AServerDispatchSwitch(uint16_t _port,// bool _close_rqst, bool _is_running,
	enum e_server_status_codes _status, int conn_timout=600):
	AServerReactive(_port, false, false, true, _status),
	_keep_alive(conn_persistance), _conn_timout(conn_timout)
{
	std::cout << "AServerDispatchSwitch constructor" << std::endl;
}

AServerDispatchSwitch::~AServerDispatchSwitch()
{
	std::cout << "AServerDispatchSwitch destructor" << std::endl;
	this->stop();
}

//if ((connfd = accept(this->_sockfd, (struct sockaddr*)&conn_addr, &addr_len)) < 0)

/// Takes clientfd of newly created client.
void
AServerDispatchSwitch::_init_new_client_connection(int clientfd)
{
	t_clt_conn			&clientconn;

	clientconn = this->_active_connections[clientfd];
	clientconn.clt_fd = clientfd;
	clientconn.conn_status = CLT_ACEPTED;
	clientconn.init_conn_time = std::time(NULL);
	clientconn.last_act_time = clientconn.init_conn_time;
	clientconn.time_on_site = 0;
	clientconn.time_inactive = 0;
}

// Receives same arguments as do_maintenance() because, in case the cluster owning this 
// server has reached its max nb of polled subjects, it will call do_maintenance() with 
// its arguments to make place for the new connecting client by disconnecting one or more.
// Returns the nb of disconnected clients and puts the clientfd in ret_clientfd.
int
AServerDispatchSwitch::connect(int *disconn_clients, int max_disconn, int *ret_clientfd)
{
	int					clientfd;
	struct sockaddr_in	clientaddr;
	socklen_t			clientaddrlen;
	int					nb_disconn = 0;

	if (!disconn_clients || !ret_clientfd)
		return (Logger::log(LOG_ERROR, "AServerDispatchSwitch::connect() method must take a pointer to an in array of size max_disconn that will potentially take a chunck of disconnected clients that needed to be bumped to make place for the new one."));


	clientaddrlen = sizeof(clientaddr);
	if ((clientfd = accept(this->_sockfd, (struct sockaddr *)&clientaddr), &clientaddrlen) < 0)
		return (Logger::log(LOG_ERROR, std::string("Server failed to accept new connection with error : ") + strerror(errno)));


	if (max_conn >= 0 && this->_active_connections.size() >= max_disconn)
	{
		nb_disconn = this->do_maintenance(disconn_clients, max_disconn);
		if (this->_active_connections.size() >= max_disconn)
		{
			nb_disconn += this->disconnect_oldest(disconn_clients + nb_disconn,
				std::min(max_disconn - nb_disconn, (this->_active_connections.size() - max_disconn) + 10);
		}		
	}
	this->_init_new_client_connection(clientfd);
	this->react(EVNT_RECEIVED_CONNECTION, clientfd);
//	this->react(EVNT_RECEIVED_REQUEST, clientfd);
	//this->serve_request(clientfd);
	*ret_clientfd = client_fd;
	return (nb_disconn);
}

int
AServerDispatchSwitch::disconnect(int clt_fd, bool force)
{
	std::map<int, t_clt_conn>::iterator it;
	std::ostringstream					err_msg;

	if (clt_fd < 3)
	{
		err_msg << "Cannot disconnect client with invalid fd ";
		err_msg << clt_fd;
		return (Logger::log(LOG_WARNING, err_msg.str()));
	}
	it = this->_active_connections.find(clt_fd);
	if (it == this->_active_connections.end()
		|| (!force && it->second.conn_status & CLT_PROCESSING))
		return (-1);
	close(clt_fd);
	this->_active_connections.erase(it);
	this->react(EVNT_CLOSING_CONNECTION, 0);// clientfd arg ignored;
	return (0);
}

void
AServerDispatchSwitch::disconnect_all(bool force)
{
	std::map<int, t_clt_conn>::iterator it;
	int									clt_fd;

	for (it=this->_active_connections.begin(); it != this->_active_connections.end(); ++it)
	{
//		if (!force && it->second.conn_status & CLT_PROCESSING);
//			continue ;
//		close(it->first);
		if (this->disconnect(it->second.clt_fd, force) == 0)
			--it;
	}
//	this->_active_connections.clear();
//		clt_fd = it->second.clt_fd;
//		if (!force && it->second.conn_status & CLT_PROCESSING)
//			continue ;
//		close(it->second.clt_fd);
//		this->_active_connections.erase(it);
//	}
}

// disconnect {max_disconn} oldest active connections.
int
AServerDispatchSwitch::disconnect_oldest(int *disconn_clients, int max_disconn)
{
	std::map<int, t_clt_conn>::iterator		it, jt;
	std::map<int, time_t>		oldest;
	time_t						longuest_time_inactive = 0;
	int							nb_disconn = 0;

	for (it = this->_active_connections.begin(); it != this->_active_connections.end(); ++it)
	{
		if (oldest.size() < max_disconn)
		{
			oldest[it->first] = it->second.time_inactive;
			longuest_time_inactive = std::max(it->second.time_inactive, longuest_time_inactive);
		}
		else if (it->second.time_inactive > longuest_time_inactive)
		{
			for (jt = oldest.begin(); jt != oldest.end(); ++jt)
			{
				if (jt->second.time_inactive < it->second.time_inactive)
				{
					oldest.erase(jt);
					oldest[it->first] = it->second.time_inactive;
				}
			}			
		}
	}
	for (jt = oldest.begin(); jt != oldest.end(); ++jt)
	{
		close(jt->first);
		disconn_clients[nb_disconn++] = jt->first;
	}
	return (nb_disconn);
}

int
AServerDispatchSwitch::do_maintenance(int *disconn_clients, int max_disconn)
{
	std::map<int, t_clt_conn>::iterator it;
	t_clt_conn							&clientconn;
	int									clientfd;
	int									nb_disconn;
	time_t								curr_time;
	bool								do_disconn;

	curr_time = std::time(NULL);
	nb_disconn = 0;
	for (it=this->_active_connections.begin(); it != this->_active_connections.end(); ++it)
	{
		it->second.time_on_site = curr_time - it->second.init_conn_time;
		it->second.time_inactive = curr_time - it->second.last_act_time;
		do_disconn = it->second.time_inactive > this->_conn_timeout;
		clientfd = it->second.clt_fd;
		if (do_disconn && (this->disconnect(clientfd, false) == 0))
		{
			if (disconn_clients && nb_disconn < max_disconn)
				disconn_clients[nb_disconn++] = clientfd;
			--it;
		}
		if (nb_disconn == max_disconn)
			break ;
	}
	this->_last_maintenance_time = std::time(NULL);
	return (nb_disconn);
}

void
AServerDispatchSwitch::stop(void)
{
	this->disconnect_all(true);
	if (this->_sockfd > 2)
		close(this->_sockfd);
	this->_status = SRV_UNBOUND;
	this->_close_request = true;
	this->react(EVNT_SRV_CLOSE, 0);// clientfd arg ignored;
}

void	AServerDispatchSwitch::switch_connection_persistance(void)
{
	if (this->_keep_alive)
		this->disconnect_all(false);
	this->_keep_alive = ~this->_keep_alive;
}

int
AServerDispatchSwitch::bind_server(void)
{
	const int enable = 1;// Used for setsockopt().

	if (this->_sockfd >= 3)
	{
		Logger::log(LOG_WARNING, "Trying to create socket while server already has a socket open. Server cannot be bound twice.");
		return (-1);
	}
	if ((this->_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return (Logger::log(LOG_ERROR, std::string("Socket creation failed with error ") + std::strerror(errno)));
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
		return (Logger::log(LOG_ERROR, std::string("Failed to set socket options with error ") + std::strerror(errno)));
	if (bind(this->_sockfd, (struct sockaddr *)&this->_server_addr, sizeof(this->_server_addr)) < 0)
	{
		close(this->_sockfd);
		return (Logger::log(LOG_ERROR, std::string("Binding socket to ip address ")
			+ inet_ntoa(this->_server_addr.sin_addr)
			+ " failed with error : " + std::strerror(errno)));
	}
	this->_status = SRV_IDLE;
	return (0);
}

bool
AServerDispatchSwitch::is_serving(int client_fd) const
{
	std::map<int, t_clt_conn>::iterator it;
	
	if (!this->_keep_alive)
		return (false);

	it = this->_active_connections.find(client_fd);
	return (it != this->_active_connections.end());
}

t_clt_conn&
AServerDispatchSwitch::get_client_state(int client_fd) const
{
	std::map<int, t_clt_conn>::iterator it;

	if (!this->_keep_alive)
		return (false);

	it = this->_active_connections.find(client_fd);
	if (it == this->_active_connections.end())
		return (nullptr);
	return (it->second);
}

uint16_t
AServerDispatchSwitch::get_port(void) const {return (this->_port);}

//int
//AServerDispatchSwitch::get_socket(void) const {return (this->_sockfd);}
/*
int
AServerDispatchSwitch::_init_macos_event_listener(void)
{
	if ((this->_pollfd = kqueue()) < 0)
		return (Logger::log(LOG_ERROR, "Failed to open kqueue fd."));
	return (0);
}
*/
int
AServerDispatchSwitch::_validate_ready_start(void)
{
	std::map<enum e_react_event, t_react_callback>::iterator	it;

	if (this->_sockfd < 3)
		return (Logger::log(LOG_WARNING, "Server has no socket yet. Call the bind_server() method first to create an AF_INET socket and bind its address and port."));

	it = this->_callbacks.find(EVNT_ACCEPTED_CONNECTION);
	if (it == this->_callbacks.end())
		return (Logger::log(LOG_WARNING, "Missing required callback for EVNT_ACCEPTED_CONNECTION event. You must register at least a callback for this event before starting the server. See register_react_callback() in AServerReact.hpp."));
	return (0);
}

// If self_managed is true, this server starts a while (1) loop and
// waits for client connections.
// Otherwise returns the server's sockfd fd to be managed externally.
int
AServerDispatchSwitch::start(void)
{
	int					connfd;
	struct sockaddr_in	conn_addr;
	socklen_t			addr_len;
	std::ostringstream	err_msg;

	if (this->_validate_ready_start())
		return (-1);

	//listen
	if (listen(this->_sockfd, MAX_PENDING_CONN) < 0)
	{
		err_msg << "Failed to start listening to port " <<  this->_port;
		err_msg << " on address " << inet_ntoa(this->_server_addr.sin_addr);
		err_msg << " with error : " << std::strerror(errno);
		return (Logger::log(LOG_ERROR, err_msg.str()));
	}
	this->_status = SRV_LISTENING;
	this->_srv_start_time = std::time(NULL);	
	
	this->react(EVNT_SRV_OPEN, 0);// clientfd arg ignored;
	return (this->_sockfd);
//	return (0);
}
/*
// If self_managed is true, this server starts a while (1) loop and
// waits for client connections.
// Otherwise returns the server's sockfd fd to be managed externally.
int
AServerDispatchSwitch::start(bool self_managed)
{
	int					connfd;
	struct sockaddr_in	conn_addr;
	socklen_t			addr_len;
	std::ostringstream	err_msg;

	if (this->_sockfd < 3)
		return (Logger::log(LOG_WARNING, "Server has no socket yet. Call the bind_server() method first to create an AF_INET socket and bind its address and port."));

	//listen
	if (listen(this->_sockfd, MAX_PENDING_CONN) < 0)
	{
		err_msg << "Failed to start listening to port " <<  this->_port;
		err_msg << " on address " << inet_ntoa(this->_server_addr.sin_addr);
		err_msg << " with error : " << std::strerror(errno);
		return (Logger::log(LOG_ERROR, err_msg.str()));
	}
	this->_status = SRV_LISTENING;

//	if (!self_managed)
//		return (this->_sockfd);
	
	this->_is_running = true;
	while (!this->_close_request)
	{
		std::memset(&conn_addr, 0, sizeof(conn_addr));
		addr_len = sizeof(conn_addr);
		std::cout << "\n\n\nAccepting new connections" << std::endl;
		
		if ((connfd = accept(this->_sockfd, (struct sockaddr*)&conn_addr, &addr_len)) < 0)
		{
			Logger::log(LOG_ERROR, std::string("accept called failed with error : ")
				+ std::strerror(errno));
			break ;
		}
		else
			std::cout << "accepted client connection with ip addr : " << inet_ntoa(conn_addr.sin_addr)
				<< " with fd : " << connfd << std::endl;
		
		char					request_buffer[4097];
		ssize_t					nc;
		std::stringstream		raw_request;
		std::string				response;
		std::cout << "reading from client socket while recv > 0" << std::endl;
		//nc = recv(connfd, request_buffer, 4096, MSG_DONTWAIT);
		nc = read(connfd, request_buffer, 4096);
		std::cout << "nc : " << nc << std::endl;
		if (nc == 0)
			std::cout << "READ RECEIVED ZERO CHARS" << std::endl;
		if (nc > 0)
		{
			if (request_buffer[0] == '\0')
				std::cout << "READ RECEIVED NULL CHARS" << std::endl;

	//		while ((nc = recv(connfd, request_buffer, 4096, MSG_DONTWAIT)) > 0)
	//		std::cout << "start while " << std::endl;
	//		while (nc > 0)
	//		{
			request_buffer[nc] = '\0';

			std::cout << "\n\n" << request_buffer << std::endl;
			raw_request << request_buffer;
	//		}
			std::cout << "get dummy response" << std::endl;
			get_dummy_response(*this, raw_request, response);

	//		std::cout << "flushing client connection down the drain" << std::endl;
	//		this->_active_connections[connfd].clt_fd = connfd;
	//		this->_active_connections[connfd].conn_status = CLT_ACCEPTED;
	//		this->_active_connections[connfd].clt_addr = conn_addr;
	//		this->_active_connections[connfd].init_conn_time = std::time(NULL);
			send(connfd, response.c_str(), response.length(), 0);
		}
//		std::cout << "shuting down client connection and fd" << std::endl;
		close(connfd);
	}
	return (0);
}
*/
