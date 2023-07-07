/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 22:18:13 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/03 20:09:15 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerHTTP.hpp"

//ServerHTTP::ServerHTTP(uint16_t port=PORT_HTTP, const std::string &rootdir="./"):
ServerHTTP::ServerHTTP(const std::string& addr, uint16_t port, const std::string &rootdir):
	AServerReactive(htons(port), false, false, SRV_UNBOUND), _rootdir(rootdir)
{
	(void)addr;
	this->_sockfd = 0;
	std::cout << "ServerHTTP Constructor" << std::endl;
	std::memset(&this->_server_addr, 0, sizeof(this->_server_addr));
	this->_server_addr.sin_family = AF_INET;
	this->_server_addr.sin_port = this->_port;
	//this->_server_addr.sin_port = _port;
	this->_server_addr.sin_addr.s_addr = INADDR_ANY;
//	if (inet_aton(addr.c_str(), &this->_server_addr.sin_addr) < 0)
	//if (this->_server_addr.sin_addr.s_addr < 0)
//		throw std::runtime_error(std::string("Network address given is invalid."));
}

ServerHTTP::~ServerHTTP(void)
{
	std::cout << "ServerHTTP Destructor" << std::endl;
	this->stop();
}

bool	ServerHTTP::_validate_client_connection(struct sockaddr_in& addr)
{
	std::map<in_addr_t, struct s_timeout_info>::iterator	to;
	std::time_t												cur_time;
	std::string												addr_str;

	if (this->_addr_blacklist.find(addr.sin_addr.s_addr) != this->_addr_blacklist.end())
	{
		addr_str = inet_ntoa(addr.sin_addr);
		Logger::log(LOG_WARNING, "Blacklisted addr " + addr_str + " tried to connect.");
		return (false);
	}
	if ((to = this->_addr_timeouts.find(addr.sin_addr.s_addr)) != this->_addr_timeouts.end())
	{
		cur_time = time(NULL);
		if (cur_time < to->second.end_time)
//		{
//			Logger::log(LOG_WARNING, "Timeout addr " + inet_ntoa(addr) + " tried to connect.");
			return (false);
//		}
		this->_addr_timeouts.erase(addr.sin_addr.s_addr);
	}
	return (true);
}

int
ServerHTTP::bind_server(void)
{
	std::cout << "ServerHTTP bind to port : " << this->_port << std::endl;
	if ((this->_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Socket creation failed with error " << std::strerror(errno) << std::endl;
		return (-1);
	}
	std::cout << "Socket created SUCCESSFULLY!\nsockfd before bind : " << this->_sockfd << std::endl;
	if (bind(this->_sockfd, (struct sockaddr *)&this->_server_addr, sizeof(this->_server_addr)) < 0)
	{
		shutdown(this->_sockfd, 2);
		std::cerr << "Binding socket to ip address " << inet_ntoa(this->_server_addr.sin_addr)
			<< " failed with error : " << std::strerror(errno) << std::endl;
		return (-1);
	}
	std::cout << "sockfd after bind : " << this->_sockfd << std::endl;
	this->_status = SRV_IDLE;
	return (0);
}


// If self_managed is true, this server starts a while (1) loop and
// waits for client connections.
// Otherwise returns the server's sockfd fd to be managed externally.
int
ServerHTTP::start(bool self_managed)
{
	int					connfd;
	struct sockaddr_in	conn_addr;
	socklen_t			addr_len;

	if (!this->_sockfd)
	{
		std::cerr << "Server has no socket yet. Call the bind_server() method first to create an AF_INET socket and bind the " << std::endl;
	}

	std::cout << "ServerHTTP listen to port : " << this->_port << std::endl;


	//listen
	std::cout << "sockfd after bind : " << this->_sockfd << std::endl;
	if (listen(this->_sockfd, MAX_PENDING_CONN) < 0)
	{
		std::cerr << "Failed to start listening to port " << ntohs(this->_port)
			<< " on address " << inet_ntoa(this->_server_addr.sin_addr)
			<< " with error : " << std::strerror(errno) << std::endl;
		return (-1);
	}
	this->_status = SRV_LISTENING;
	this->_is_running = true;

	if (!self_managed)
		return (this->_sockfd);
	
	while (!this->_close_request)
	{
		std::memset(&conn_addr, 0, sizeof(conn_addr));

		addr_len = sizeof(conn_addr);
		if ((connfd = accept(this->_sockfd, (struct sockaddr*)&conn_addr, &addr_len)) < 0)
		{
			std::cerr << "accept called failed with error : " << std::strerror(errno) << std::endl;
			break ;
		}
		else
			std::cout << "accepted client connection with ip addr : " << inet_ntoa(conn_addr.sin_addr)
				<< " with fd : " << connfd << std::endl;
		if (!this->_validate_client_connection(conn_addr))
		{
			shutdown(connfd, 2);
			continue ;
		}
		std::cout << "flushing client connection down the drain" << std::endl;
//		this->_active_connections[connfd].clt_fd = connfd;
//		this->_active_connections[connfd].conn_status = CLT_ACCEPTED;
//		this->_active_connections[connfd].clt_addr = conn_addr;
//		this->_active_connections[connfd].init_conn_time = std::time(NULL);

		char	receive_buff[4096];

		ssize_t	nchr = read(connfd, receive_buff, 4096);
		receive_buff[nchr] = '\0';
		std::string recv_str = receive_buff;
		std::cout << "Request received : \n" << std::endl;
		std::cout << recv_str << std::endl;
		// Send dummy response
		std::string	dummy = "HTTP/1.0 200 OK\r\n\r\n";
		dummy += "<h1>HELLO WORLD</h1>";
		std::cout << "Sending response : \n" << std::endl;
		std::cout << dummy << std::endl;

		send(connfd, dummy.c_str(), dummy.length(), 0);
		shutdown(connfd, 2);// FOR DEBUG ONLY !!
		close(connfd);
	}
	return (0);
}

void
ServerHTTP::disconnect(int clt_fd)
{
	std::map<int, t_clt_conn>::iterator it;// = this->_active_connections.begin();

	it = this->_active_connections.find(clt_fd);
	if (it == this->_active_connections.end())
		return ;

	shutdown(clt_fd, 2); 
	this->_active_connections.erase(clt_fd);
}

void
ServerHTTP::stop(void)
{
	std::map<int, t_clt_conn>::iterator it;

	std::cout << "Try stop ServerHTTP listening on port : " << ntohs(this->_port) << std::endl;
	for (it=this->_active_connections.begin(); it != this->_active_connections.end();)// it++)
	{
		shutdown(it->second.clt_fd, 2);
		this->_active_connections.erase(it++);
	}
	shutdown(this->_sockfd, 2);
	
	this->_status = SRV_UNBOUND;
	this->_close_request = true;
}

t_srv_state*
ServerHTTP::get_srv_state(void)
{
	this->_srv_state_view.port = ntohs(this->_port);
	this->_srv_state_view.address = inet_ntoa(this->_server_addr.sin_addr);
	this->_srv_state_view.is_running = this->_is_running;
	srv_status_to_str(this->_status, this->_srv_state_view.status);
	this->_srv_state_view.extra = NULL;
	return (&this->_srv_state_view);
}

std::ostream&	operator<<(std::ostream& ostream, ServerHTTP& srv)
{
	t_srv_state	*state = srv.get_srv_state();

	std::cout << "<--- HTTP Server state ---------------->" << std::endl;
	std::cout << "| - is running : \t" << (state->is_running ? "TRUE" : "FALSE") << std::endl;
	std::cout << "| - port : \t\t" << state->port << std::endl;
	std::cout << "| - address : \t\t" << state->address << std::endl;
	std::cout << "| - status : \t\t" << state->status << std::endl;
	std::cout << "<------------------------------------->" << std::endl;
	return (ostream);
}
