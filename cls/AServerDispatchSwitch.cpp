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

AServerDispatchSwitch::AServerDispatchSwitch(uint16_t _port, bool _close_rqst,
	bool _is_running, enum e_server_status_codes _status, 
	bool conn_persistance, int conn_timout=600):
	AServerReactive(_port, _close_rqst, _is_running, _status),
	_keep_alive(conn_persistance), _conn_timout(conn_timout)
{
	std::cout << "AServerDispatchSwitch constructor" << std::endl;
}

AServerDispatchSwitch::~AServerDispatchSwitch()
{
	std::cout << "AServerDispatchSwitch destructor" << std::endl;
	this->stop();
}

void
AServerDispatchSwitch::disconnect(int clt_fd, bool force)
{
	std::map<int, t_clt_conn>::iterator it;

	it = this->_active_connections.find(clt_fd);
	if (it == this->_active_connections.end()
		|| (!force && it->second.conn_status & CLT_PROCESSING))
		return ;
	if (clt_fd > 2)
		close(clt_fd);
	this->_active_connections.erase(clt_fd);
}

void
AServerDispatchSwitch::disconnect_all(bool force)
{
	std::map<int, t_clt_conn>::iterator it;
	int									clt_fd;

	for (it=this->_active_connections.begin(); it != this->_active_connections.end(); it++)
//	{
		this->disconnect(it->second.clt_fd, force);
//		clt_fd = it->second.clt_fd;
//		if (!force && it->second.conn_status & CLT_PROCESSING)
//			continue ;
//		close(it->second.clt_fd);
//		this->_active_connections.erase(it);
//	}
}

void
AServerDispatchSwitch::stop(void)
{
	this->disconnect_all();
	if (this->_sockfd > 2)
		close(this->_sockfd);
	this->_status = SRV_UNBOUND;
	this->_close_request = true;

	// close epoll / kqueue;
}

void	AServerDispatchSwitch::switch_connection_persistance(void)
{
	if (this->_keep_alive)
		this->disconnect_all();
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

int
AServerDispatchSwitch::get_socket(void) const {return (this->_sockfd);}

int
AServerDispatchSwitch::_init_macos_event_listener(void)
{
	if ((this->_pollfd = kqueue()) < 0)
		return (Logger::log(LOG_ERROR, "Failed to open kqueue fd."));
	return (0);
}

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
