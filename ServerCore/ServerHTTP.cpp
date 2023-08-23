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
	std::cout << "ServerHTTP constructor" << std::endl;

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

ServerHTTP::~ServerHTTP(void) {std::cout << "ServerHTTP desctructor" << std::endl;}

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

//	this->react(EVNT_SRV_OPEN, 0);// clientfd arg ignored;
	return (this->_sockfd);
}



#define MAX_READ_BUFF 4096

int
ServerHTTP::receive_request(int clientfd, Request& request)
{
	char				request_buff[MAX_READ_BUFF + 1];
	ssize_t				read_size;
	bool				client_is_late = false;

	std::cout << "Starting to read client socket until return <= 0" << std::endl;
// 	while ((read_size = read(clientfd, request_buff, MAX_READ_BUFF)) > 0)
// 	{
// 		std::cout << "reading chunk with read_size : " << read_size << std::endl;
// 		request_buff[read_size] = '\0';
// 		request << request_buff;
// //		req_str += request_buff;
// 	}
	std::cout << "request content length at start : " << request.get_content_length() << std::endl;
	while (1)
	{
		if (request.is_header_parsed()
			&& (request.length() - request.get_header_length()) == request.get_content_length())
			break ;
//		std::cout << "Reading one chunk " << std::endl;
		read_size = read(clientfd, request_buff, MAX_READ_BUFF);
//		std::cout << "read_size  : " << read_size << ", content length : " << request.get_content_length() << std::endl;
//		std::cout << "_raw_request length : " << request.length() << std::endl;
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
//		std::cout << "reading chunk with read_size : " << read_size << std::endl;
//		request_buff[read_size] = '\0';
		//request << request_buff;
		request.append(request_buff, read_size);
		usleep(100);
	}

//	std::cout << "Finished reading client socket with read_size : " << read_size << std::endl;
	if (read_size == 0)
	{
		Logger::log(LOG_DEBUG, "Client has disconnected. Closing client socket.");
		return (-1);
	}
//	if (read_size < 0)
//	{
//		Logger::log(LOG_WARNING, "Reading from client socket returned -1.");
//		return (-1);
//	}
	// std::cout << "Read " << request.length() << " chars from client." << std::endl;
	// std::cout << "** ------------------------ [RECEIVED REQUEST] -------------------------- **" << std::endl;
	// std::cout << request << std::endl;
	// std::cout << "** ---------------------- [RECEIVED REQUEST END] -------------------------- **" << std::endl;
//	if (req_str.length() == 0)
//		return (0);
	return (0);
}

/*
int
ServerHTTP::parse_request(Request& request) const
{
	if (request.process_raw_request() < 0)
		return (-1);
	return (0);
}
*/


/// This method should be noexcept. As long as clientfd is open, should either send
/// the response or an error page.
int
ServerHTTP::send_response(int clientfd, const Response& resp) const
{
	std::cout << "Sending response page" << std::endl;
//	char				request_buff[MAX_READ_BUFF + 1];
//	ssize_t				read_size, send_size;
//	std::string			header;
//	std::string			body;
//	std::ostringstream	content_length;
//	std::string&		response;
	const std::string&		response = resp.get_response();
	ssize_t					sent_size;
	size_t					to_send = response.length();
	const char*				msg = response.c_str();
	int						max_retries = SRVHTTP_MAX_SEND_RETRIES;

//	std::cout << "Server Sending response size : " << to_send << std::endl;
	while (to_send > 0)
	//while ((send_size = write(clientfd, response.c_str(), to_send)) > 0)
	{
//		std::cout << "Sending one chunk " << std::endl;
		if ((sent_size = write(clientfd, msg, to_send)) < 0)
		{
			if (max_retries <= 0)
			{
				Logger::log(LOG_WARNING, "Client disconnected before full response could be sent.");
				return (-1);;
			}
			--max_retries;
			usleep(10000);
			continue ;
		}
		if (_client_disconnect_signaled)
			return (-1);
		max_retries = SRVHTTP_MAX_SEND_RETRIES;
//		std::cout << sent_size << " successfully sent." << std::endl;
		to_send -= sent_size;
		msg += sent_size;
	}

/*
	if ((send_size = write(clientfd, response.c_str(), response.length())) < 0)
	{
		std::cerr << "Write failed" << std::endl;
		return (Logger::log(LOG_ERROR, "failed to write() client request after read()."));
	}
	std::cout << "Response page SENT !" << std::endl;
*/
//	std::cout << "Full Response successfully SENT !" << std::endl;
	return (0);
}

int	ServerHTTP::_serve_internal_error(int clientfd, const Request& req, const ServerConfig& cfg) const
{
	ErrorResponse		err(*this, req, cfg);

	std::cerr << "Serving Internal Error Response Page " << std::endl;
	err.prepare_response(500);
	std::cout << err.get_response() << std::endl;
	this->send_response(clientfd, err);
	return (-1);
}

/// Full service function wrapping together the whole process of servicing a client request.
int
ServerHTTP::serve_request(int clientfd)
{
	Request				req;
	Response			resp;
//	const std::string	*requested_host;

	std::cout << std::endl << std::endl << "*------------------------{SERVER RECEIVED NEW REQUEST}-------------------------*" << std::endl;
	
	_currently_serving = clientfd;
	if (receive_request(clientfd, req) < 0)
	{
		_currently_serving = 0;
		return (Logger::log(LOG_DEBUG, "Client disconnected. Closing clientfd."), -1);
	}
	if (req.process_raw_request() < 0)
	{
		Logger::log(LOG_ERROR, "Processing request failed. Disconnecting client.");
		_currently_serving = 0;
		return (_serve_internal_error(clientfd, req, this->_cfgs[0]));
	}

	/// DEBUG DELETE ME
//	return (_serve_internal_error(clientfd, req, this->_cfgs[0]));
//	std::cout << "Request : " << std::endl;
//	std::cout << req << std::endl;
//	std::cout << std::endl << "*-------------- REQUEST HEADER ----------------*" << std::endl;
//	std::cout << req << std::endl;
//	std::cout << std::endl << "*-------------- REQUEST HEADER END ----------------*" << std::endl;

	const ServerConfig&	cfg = get_config_for_host(req["Host"]);

	if (resp.prepare_response(*this, req, cfg) < 0)
	{
		/// SEND ERROR PAGE
//		std::cerr << "Prepare response failed " << std::endl;
		int	error_code = resp.get_error_code();
		ErrorResponse		err(*this, req, cfg);
		if (error_code)
		{
//			std::cout << "PREPARING ERROR RESPONSE PAGE AFTER prepare_response() FAILED !" << std::endl;
			err.prepare_response(error_code);
		}
		else
			err.prepare_response(500);
//		send_response(clientfd, err);
//		return (_serve_internal_error(clientfd, req, cfg));
		send_response(clientfd, err);
	}
	else
	{
//		std::cout << "Seems like prepare_response() was executed SUCCESSFULLY" << std::endl;
		send_response(clientfd, resp);
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

	
//	std::cout << "Looking for location config in the " << _cfgs.size() << " configs managed by this server." << std::endl;
	for (cfgs_it = _cfgs.begin(); cfgs_it != _cfgs.end(); ++cfgs_it)
	{
//		std::cout << "Comparing location server_name " << cfgs_it->GetServerName() << " vs " << host << std::endl;
		pos = host.find_last_of(':');
//		if (pos != std::string::npos)
//		{
//			std::cout << "req host up to ':' char : " << host.substr(0, pos) << ", pos : " << pos << std::endl;
//			std::cout << "req host cmp srv_name : " << host.compare(0, pos, cfgs_it->GetServerName()) << std::endl;
//			std::cout << "req host name up to pos : '" << host.substr(0, pos) << "'" << std::endl;
//			std::cout << "req srv_name name up to pos : '" << host.substr(0, pos) << "'" << std::endl;
//		}
//		std::cout << "Does requested host (" << host << ") == server_name (" << cfgs_it->GetServerName() << ") ? " << std::boolalpha << (cfgs_it->GetServerName() == host) << std::endl;
		if (cfgs_it->GetServerName() == host)
			return (*cfgs_it);
//		if (pos == std::string::npos && cfgs_it->GetServerName() == host)
//			return (*cfgs_it);
//		std::cout << "Does requested host (" << host << ") compare to server_name (" << cfgs_it->GetServerName() << ") ? " << std::boolalpha  << (host.compare(0, pos, cfgs_it->GetServerName()) == 0) << std::endl;
		if (pos != std::string::npos
			&& host.compare(0, pos, cfgs_it->GetServerName()) == 0)
			return (*cfgs_it);
	}
//	std::cout << "Serving default location config :" << std::endl;
	return (*_cfgs.begin());
}

bool
//ServerHTTP::add_virtual_server(const ServerHTTP& other)
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
//	_cfgs.insert(_cfgs.end(), other._cfgs.begin(), other._cfgs.end());
	_cfgs.insert(_cfgs.end(), other.get_config().begin(), other.get_config().end());
	//_cfgs.push_back(other._cfg);
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

//const std::map<std::string, std::string>&
//ServerHTTP::get_srv_locations(void) const {return (this->_locations);}


// std::ostream&	operator<<(std::ostream& ostream, ServerHTTP& srv)
// {
// 	t_srv_state	*state = srv.get_srv_state();

// 	std::cout << "<--- HTTP Server state ---------------->" << std::endl;
// 	std::cout << "| - is running : \t" << (state->is_running ? "TRUE" : "FALSE") << std::endl;
// 	std::cout << "| - port : \t\t" << state->port << std::endl;
// 	std::cout << "| - address : \t\t" << state->address << std::endl;
// 	std::cout << "| - status : \t\t" << state->status << std::endl;
// 	std::cout << "<------------------------------------->" << std::endl;
// 	return (ostream);
// }

//int
//ServerHTTP::bind_server(void) {return (0);}

