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


ServerHTTP::ServerHTTP(const std::string& servname, const std::string& rootdir,
	const std::string& ip, uint16_t port, int timeout, const ServerConfig* cfg):
	AServerDispatchSwitch(port, SRV_UNBOUND, true, timeout),
	_rootdir(rootdir)
{
	in_addr_t			addr;
	std::ostringstream	os;

	os << port;
	_server_name = servname;
	if (ip != "0.0.0.0")
	{
		if ((addr = inet_addr(ip.c_str())) != INADDR_NONE)
			this->_server_addr.sin_addr.s_addr = addr;
		else
			Logger::log(LOG_ERROR, std::string("Given Listen IP address is invalid : ") + ip);
	}
	if (!cfg)
		Logger::log(LOG_WARNING, std::string("Server on ") + ip + ":" + os.str() + " started without ServerConfig and might not behave as expected.");
	else
		_cfgs.push_back(*cfg);
}

ServerHTTP::~ServerHTTP(void)
{
}

int
ServerHTTP::start(void)
{
	std::ostringstream	err_msg;

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

	this->react(EVNT_SRV_OPEN, 0);
	return (this->_sockfd);
}


int
ServerHTTP::receive_request(int clientfd, Request& request)
{
	char				request_buff[MAX_READ_BUFF + 1];
	ssize_t				read_size;
	bool				client_is_late = false;

	while (1)
	{
		if (request.is_header_parsed()
			&& (request.length() - request.get_header_length()) == request.get_content_length())
			break ;
		read_size = read(clientfd, request_buff, MAX_READ_BUFF);
		if (read_size == 0)
			break ;
		else if (read_size < 0)
		{
			if (client_is_late && request.get_content_length() == 0)
				break ;
			client_is_late = true;
			usleep(10000);
			continue;
		}
		client_is_late = false;
		request.append(request_buff, read_size);
		usleep(100);
	}

	if (read_size == 0)
		return (-1);
	return (0);
}


/// As long as clientfd is open, should either send
/// the response or an error page.
int
ServerHTTP::send_response(int clientfd, const Response& resp) const
{
	const std::string&		response = resp.get_response();
	ssize_t					sent_size;
	size_t					to_send = response.length();
	const char*				msg = response.c_str();
	int						max_retries = SRVHTTP_MAX_SEND_RETRIES;

	while (to_send > 0)
	{
		if ((sent_size = write(clientfd, msg, to_send)) < 0)
		{
			if (max_retries <= 0)
			{
				Logger::log(LOG_WARNING, "Client disconnected before full response could be sent.");
				return (-1);;
			}
			--max_retries;
			usleep(100000);
			continue ;
		}
		if (sent_size == 0 || _client_disconnect_signaled)
			return (-1);
		max_retries = SRVHTTP_MAX_SEND_RETRIES;
		to_send -= sent_size;
		msg += sent_size;
	}
	return (0);
}

int	ServerHTTP::_serve_internal_error(int clientfd, const Request& req, const ServerConfig& cfg) const
{
	ErrorResponse		err(*this, req, cfg);

	std::cerr << "Serving Internal Error Response Page " << std::endl;
	err.prepare_response(500);
	this->send_response(clientfd, err);
	return (-1);
}

/// Full service function wrapping together the whole process of servicing a client request.
int
ServerHTTP::serve_request(int clientfd)
{
	Request				req;
	Response			resp;

	_currently_serving = clientfd;
	if (receive_request(clientfd, req) < 0)
	{
		_currently_serving = 0;
		return (-1);
	}
	if (req.process_raw_request() < 0)
	{
		Logger::log(LOG_ERROR, "Processing request failed. Disconnecting client.");
		_currently_serving = 0;
		return (_serve_internal_error(clientfd, req, this->_cfgs[0]));
	}

	const ServerConfig&	cfg = get_config_for_host(req["Host"]);

	if (resp.prepare_response(req, cfg) < 0)
	{
		int	error_code = resp.get_error_code();
		ErrorResponse		err(*this, req, cfg);
		if (error_code)
			err.prepare_response(error_code);
		else
			err.prepare_response(500);
		if (send_response(clientfd, err) < 0)
			this->disconnect(clientfd, true);
	}
	else
	{
		if (send_response(clientfd, resp) < 0)
			this->disconnect(clientfd, true);
	}
	_currently_serving = 0;
	return (0);
}


t_srv_state*
ServerHTTP::get_srv_state(void)
{
	this->_srv_state_view.port = this->_port;
	this->_srv_state_view.address = inet_ntoa(this->_server_addr.sin_addr);
	this->_srv_state_view.is_running = this->_is_running;
	this->_srv_state_view.status_code = this->_status;
	srv_status_to_str(this->_status, this->_srv_state_view.status);
	this->_srv_state_view.extra = NULL;
	return (&this->_srv_state_view);
}

const std::string& ServerHTTP::get_server_name(void) const {return (this->_server_name);}
const std::string& ServerHTTP::get_rootdir(void) const {return (this->_rootdir);}


const ServerConfig&
ServerHTTP::get_config_for_host(const std::string& host) const
{
	std::vector<ServerConfig>::const_iterator		cfgs_it;
	size_t	pos;

	
	for (cfgs_it = _cfgs.begin(); cfgs_it != _cfgs.end(); ++cfgs_it)
	{
		pos = host.find_last_of(':');
		if (cfgs_it->GetServerName() == host)
			return (*cfgs_it);
		if (pos != std::string::npos
			&& host.compare(0, pos, cfgs_it->GetServerName()) == 0)
			return (*cfgs_it);
	}
	return (*_cfgs.begin());
}

bool
ServerHTTP::add_virtual_server(const IServer& other)
{
	std::vector<ServerConfig>::iterator			cfgs_it;

	if (!(*this == other))
		return (Logger::log(LOG_ERROR, "Failed to merge 2 servers. Merging servers requires that both share the same network interface."), false);
	for (cfgs_it = _cfgs.begin(); cfgs_it != _cfgs.end(); ++cfgs_it)
	{
		if (other.get_server_name() == cfgs_it->GetServerName())
			return (Logger::log(LOG_ERROR, "Failed to merge 2 servers. Cannot Merge servers on same network interface, responding to requests to the same host. Would overwrite previous config."), false);
	}
	_cfgs.insert(_cfgs.end(), other.get_config().begin(), other.get_config().end());
	return (true);
}

const std::vector<LocationConfig>&
ServerHTTP::get_srv_locations(void) const {return (_cfgs[0].GetLocations());}

const std::vector<LocationConfig>*
ServerHTTP::get_srv_locations(const std::string& host) const
{
	std::vector<ServerConfig>::const_iterator	cfgs_it = _cfgs.begin();

	for (; cfgs_it != _cfgs.end(); ++cfgs_it)
		if (cfgs_it->GetServerName() == host)
			return (&cfgs_it->GetLocations());
	return (&_cfgs[0].GetLocations());
}
