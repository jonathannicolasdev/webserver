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

#include <vector>
#include <string>
#include <dirent.h>

#include "webserv.hpp"
#include "Request.hpp"
#include "ServerHTTP.hpp"

/*
void	print_current_dir_files()
{
	DIR				*dir;
	struct dirent	*direntry;
	std::ifstream	wow;

	dir = opendir("./www/assets");
	if (!dir)
		return ;
	
	while ((direntry = readdir(dir)))
	{
		std::cout << direntry->d_name << std::endl;
		// if (std::string(direntry->d_name) == "asdf.jpg")
		// {
		// 	wow.open(std::string("./www/assets/") + direntry->d_name,
		// 	std::ios::in | std::ios::binary);
		// 	if (!wow.is_open())
		// 		std::cout << "wow " << std::string("./www/assets/") + direntry->d_name
		// 			<< " failed to load." << std::endl;
		// 	else
		// 		std::cout << wow.rdbuf();
		// }
	}
}

int	process_request_header(std::stringstream &file, std::vector<std::string> &header_tab,
	std::string &method, std::string &path, std::string &protocol)
{
	std::string	line;
	std::string	temp;
	size_t		p, p2;

	while (std::getline(file, line))
		header_tab.push_back(line);

	if (!header_tab[0].empty())
	{
		temp = header_tab[0];
		p = temp.find(" ");
		method = temp.substr(0, p);
		std::cout << "method : " << method << std::endl;
		p2 = temp.find(" ", p + 1);
		path = temp.substr(p + 1, p2 - p - 1);
		p = p2;
		std::cout << "path : " << path << std::endl;
		std::cout << "path lenght : " << path.length() << std::endl;
		p2 = temp.find_first_of(" \t\n\r", p + 1);
		protocol = temp.substr(p + 1, p2 - p - 1);
		std::cout << "protocol : " << protocol << std::endl;

	}
	return (0);
}

std::string&	prepare_response_header(const std::string &hostname,
	std::string &content_type, size_t content_size, std::string& header)
{
	std::string				timestamp;
	std::ostringstream		content_lenght_str;

	gen_timestamp(timestamp);
	header = "HTTP/1.1 200 OK\n";
	header += "Host: " + hostname + "\n";
	//header += "Date: Tuesday, 25-Nov-97 01:22:04 GMT\n";
	header += "Date: " + timestamp + "\n";//Tuesday, 25-Nov-97 01:22:04 GMT\n";
//	header += "Last-modified: Thursday, 20-Nov-97 10:44:53 GMT\n";
	header += "Content-type: " + content_type + "\n";//text/html\n";
	header += "Content-length: ";
	content_lenght_str << content_size;
	header += content_lenght_str.str();
	header += "\r\n\r\n";
	return (header);
}

void	print_req_header(std::vector<std::string> &header_tab)
{
	std::vector<std::string>::iterator	it;

	std::cout << "\nRequest Header : " << std::endl;
	for (it = header_tab.begin(); it != header_tab.end(); it++)
		std::cout << *it.base() << std::endl;
}

// Give the a stream containing the raw request from the client and an empty string
// object. The dummy response will be put in this empty string object.
std::string&	get_dummy_response(ServerHTTP &srv, std::stringstream& raw_request, std::string& response)
{
//	char	receive_buff[4096];
//	ssize_t	nchr;
	std::vector<std::string>	req_header_tags;
	std::string					filepath;
	std::ifstream				file;
	std::string					header, content, method, path, protocol;
	std::string					content_type = "text/html";
	std::ostringstream			ss;

	std::cout << "Getting dummy response" << std::endl;
	ss.clear();
	response.clear();
	
///	(void)raw_request;
	if (process_request_header(raw_request, req_header_tags, method, path, protocol) < 0)
	{
		Logger::log(LOG_ERROR, "processing request header failed");
		file.close();
		return (response);
	}
	print_req_header(req_header_tags);
	if (method == "GET")
	{
		std::cout << "path.find(.jpg) : " << path.find(".jpg") << std::endl;
		std::cout << "path compare last char to / : " << (path.at(path.length() - 1) == '/') << std::endl;
		std::cout << "last path char : " << path.at(path.length() - 1) << std::endl;
		if (path.at(path.length() - 1) == '/')
//		if (*(path.end() - 1).base() == '/')// && path == "/")
			filepath = srv.get_rootdir() + "/index.html";
		else if (path == "/favicon.ico")
		{
			filepath = srv.get_rootdir() + path;
			content_type = "image/x-icon";
		}
		else if (path.find(".jpg") != path.npos)
		{
			filepath = srv.get_rootdir() + "/assets" + path;
			content_type = "image/jpeg";
		}		
		else
			filepath = srv.get_rootdir() + path;

		std::cout << "PWD : " << get_working_path() << std::endl;
		if (access(filepath.c_str(), F_OK) < 0)
			std::cout << "file path " << filepath.c_str() << "cannot be found." << std::endl;
		if (access(filepath.c_str(), R_OK) < 0)
			std::cout << "file path " << filepath.c_str() << "cannot be read." << std::endl;
		std::cout << "open response filepath : " << filepath << std::endl;
		file.open(filepath, std::ios::in | std::ios::binary);//"index.html");
	}
//	else
		// RESPONDE WITH GENERIC 404 ERROR PAGE

//	file.open("./www/index.html");
	if (!file.is_open())
	{
		std::cerr << "File failed to open with error : " << strerror(errno)  << std::endl;
		content = "<h1>HELLO WORLD</h1>";
	}
	else
	{
		ss << file.rdbuf();
		content = ss.str();
		file.close();
	}

	prepare_response_header(srv.get_server_name(), content_type, content.length(), header);
	std::cout << "\nPrepared response header : " << std::endl;
	std::cout << header << std::endl;
	// Prepare DUMMY header ()
	// header = "HTTP/1.1 200 OK\n";
	// header += "Host: JambonCity\n";
	// header += "Date: Tuesday, 25-Nov-97 01:22:04 GMT\n";
	// header += "Last-modified: Thursday, 20-Nov-97 10:44:53 GMT\n";
	// header += "Content-type: text/html\n";

	response = header + content;
	
//	std::cout << "Sending response : \n" << std::endl;
//	std::cout << response << std::endl;
	return (response);
}
*/
/*
//ServerHTTP::ServerHTTP(uint16_t port=PORT_HTTP, const std::string &rootdir="./"):
ServerHTTP::ServerHTTP(const std::string& srv_name, const std::string& addr, 
	uint16_t port, const std::string &rootdir):
	AServerReactive(port, false, false, SRV_UNBOUND),
	_rootdir(get_working_path() + rootdir), _server_name(srv_name)
{
	(void)addr;
	this->_sockfd = 0;
	std::cout << "ServerHTTP Constructor" << std::endl;
	std::memset(&this->_server_addr, 0, sizeof(this->_server_addr));
	this->_server_addr.sin_family = AF_INET;
	this->_server_addr.sin_port = htons(this->_port);
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
	if (this->_sockfd >= 3)
	{
		Logger::log(LOG_WARNING, "Trying to create socket while server already has a socket open. Server cannot be bound twice.");
		return (-1);
	}

	std::cout << "ServerHTTP bind to port : " << this->_port << std::endl;
	if ((this->_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//	{
		return (Logger::log(LOG_ERROR, std::string("Socket creation failed with error ") + std::strerror(errno)));
//		std::cerr << "Socket creation failed with error " << std::strerror(errno) << std::endl;
//	}

	const int enable = 1;
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		return (Logger::log(LOG_ERROR, std::string("Failed to set socket options with error ") + std::strerror(errno)));
//		error("setsockopt(SO_REUSEADDR) failed");

	std::cout << "Socket created SUCCESSFULLY!\nsockfd before bind : " << this->_sockfd << std::endl;
	if (bind(this->_sockfd, (struct sockaddr *)&this->_server_addr, sizeof(this->_server_addr)) < 0)
	{
//		shutdown(this->_sockfd, 2);
		close(this->_sockfd);
		Logger::log(LOG_ERROR, std::string("Binding socket to ip address ") + inet_ntoa(this->_server_addr.sin_addr)
			+ " failed with error : " + std::strerror(errno));
	//	std::cerr << "Binding socket to ip address " << inet_ntoa(this->_server_addr.sin_addr)
	//		<< " failed with error : " << std::strerror(errno) << std::endl;
		return (-1);
	}
	std::cout << "sockfd after bind : " << this->_sockfd << std::endl;
	this->_status = SRV_IDLE;


	//// DEBUG 
//	print_current_dir_files();

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
	std::ostringstream	err_msg;

	if (this->_sockfd < 3)
		return (Logger::log(LOG_WARNING, "Server has no socket yet. Call the bind_server() method first to create an AF_INET socket and bind its address and port."));

	std::cout << "ServerHTTP listen to port : " << this->_port << std::endl;
	//listen
	if (listen(this->_sockfd, MAX_PENDING_CONN) < 0)
	{
		err_msg << "Failed to start listening to port " <<  this->_port;
		err_msg << " on address " << inet_ntoa(this->_server_addr.sin_addr);
		err_msg << " with error : " << std::strerror(errno);
		return (Logger::log(LOG_ERROR, err_msg.str()));
	}
	this->_status = SRV_LISTENING;
	this->_close_request = false;

	if (!self_managed)
		return (this->_sockfd);
	
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
//			std::cerr << "accept called failed with error : " << std::strerror(errno) << std::endl;
			break ;
		}
		else
			std::cout << "accepted client connection with ip addr : " << inet_ntoa(conn_addr.sin_addr)
				<< " with fd : " << connfd << std::endl;
		
		if (!this->_validate_client_connection(conn_addr) && (close(connfd), 1))
//		{
//			close(connfd);
			continue ;
//		}

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
//		shutdown(connfd, 2);// FOR DEBUG ONLY !!
//		close(connfd);
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

//	shutdown(clt_fd, 2);
	if (clt_fd > 2)
		close(clt_fd);
	this->_active_connections.erase(clt_fd);
}

void
ServerHTTP::stop(void)
{
	std::map<int, t_clt_conn>::iterator it;

	std::cout << "Try stop ServerHTTP listening on port : " << this->_port << std::endl;
	for (it=this->_active_connections.begin(); it != this->_active_connections.end();)// it++)
	{
		if (it->second.clt_fd > 2)
			close(it->second.clt_fd);
		this->_active_connections.erase(it++);
	}
	if (this->_sockfd > 2)
		close(this->_sockfd);
	this->_status = SRV_UNBOUND;
	this->_close_request = true;
}
*/

ServerHTTP::ServerHTTP(const std::string& rootdir, const std::string& servname,
	const std::string& ip, uint16_t port):
	AServerDispatchSwitch(_port, SRV_UNBOUND, true),
	_rootdir(rootdir), _server_name(servname)
{
	UNUSED(ip);
	UNUSED(port);
	std::cout << "ServerHTTP constructor" << std::endl;
}

ServerHTTP::~ServerHTTP(void) {std::cout << "ServerHTTP desctructor" << std::endl;}

// DEBUG PURPOSES ONLY
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
	
	Request		rq;
	//Response	resp;

	while (1)
	{
		addr_len = sizeof(struct sockaddr_in);
		if ((connfd = accept(this->_sockfd, (struct sockaddr *)&conn_addr, &addr_len)) < 0)
			return (Logger::log(LOG_ERROR, std::string("accept call failed with error : ") + strerror(errno)));
		
		this->_init_new_client_connection(connfd);
		this->parse_request(connfd, rq);
//		this->prepare_response(connfd, rq, );
		close(connfd);
	}
//	this->react(EVNT_SRV_OPEN, 0);// clientfd arg ignored;
//	return (this->_sockfd);
//	return (0);
}

#define MAX_READ_BUFF 4096
int
ServerHTTP::parse_request(int clientfd, Request& request) const
{
	char				request_buff[MAX_READ_BUFF + 1];
	ssize_t				read_size, send_size;
	std::string			req_str;
	std::string			header;

	UNUSED(request);
	while ((read_size = read(clientfd, request_buff, MAX_READ_BUFF)) == MAX_READ_BUFF)
	{
		request_buff[read_size] = '\0';
		req_str += request_buff;
	}
	if (read_size == -1)
		return (Logger::log(LOG_ERROR, "failed to read() client request after accept()."));
	request_buff[read_size] = '\0';
	req_str += request_buff;
	header = "HTTP/1.1 200 OK\n";
	if ((send_size = write(clientfd, header.c_str(), header.length())) < 0)
		return (Logger::log(LOG_ERROR, "failed to write() client request after read()."));
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

const std::map<std::string, std::string>&
ServerHTTP::get_srv_locations(void) const {return (this->_locations);}


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

//int
//ServerHTTP::bind_server(void) {return (0);}

int main()
{
	ServerHTTP	srv("www/", "Jimbo jones", "127.0.0.1", 80);

	if (srv.bind_server() < 0)// || srv.)
		return (1);

	return (0);
}
