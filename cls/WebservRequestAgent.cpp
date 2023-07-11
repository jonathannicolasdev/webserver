/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WeservRequestAgent.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 18:56:13 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/10 20:38:04 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservRequestAgent.hpp"

WebservRequestAgent::WebservRequestAgent()
{
	std::cout << "WebservRequestAgent constructor" << std::endl;
	this->_req_content.clear();
	if (pipe(this->_pipe) < 0)
		throw std::ios_base::failure("Pipe initialization failed");
}

WebservRequestAgent::~WebservRequestAgent()
{
	std::cout << "WebservRequestAgent destructor" << std::endl;
}
/*
size_t WebservRequestAgent::get_content_length(std::stringstream& content)
{
	size_t  size;

	content.seekg(0, std::ios::end);
	size = content.tellg();
	content.seekg(0, std::ios::beg);
	return (size);
}
*/

// Returns the write side of the agents pipe 
int	WebservRequestAgent::register_server_to_agent(IServer& srv)
{
	t_srv_state	*srv_state;

	srv_state = srv.get_srv_state();
	if (srv_state->status_code & SRV_UNBOUND || srv_state->status_code & SRV_BROKEN)
		return (Logger::log(LOG_WARNING, "Cannot register unbound or broken server to a WebservRequestAgent."));
	this->_server_location_cfgs[srv_state->port] = srv.get_srv_locations();
	return (this->_pipe[1]);
}

int	WebservRequestAgent::get_response_pipe(void) const {return (this->_pipe[0]);}

int	WebservRequestAgent::push_request(int sockfd, int connfd, const std::string req_content)
{
	t_ws_req	send_rqst_header;

	send_rqst_header.sockfd = sockfd;
	send_rqst_header.connfd = connfd;
	send_rqst_header.content_size = req_content.length();//this->get_content_length(req_content);
	if (send(this->_pipe[1], &send_rqst_header, sizeof(send_rqst_header), MSG_DONTWAIT) < 0
		|| send(this->_pipe[1], req_content.c_str(), send_rqst_header.content_size, MSG_DONTWAIT) < 0)
	{
		if (errno & EWOULDBLOCK)
			return (Logger::log(LOG_WARNING, "Failed to push request. WebservRequestAgent too busy."));
		else
			return (Logger::log(LOG_WARNING, "Failed to push request to WebservRequestAgent."));
	}
	
	return (0);
}

int	process_request_header(std::stringstream &file,
	std::vector<std::string> &header_tab,
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
/*
void	print_req_header(std::vector<std::string> &header_tab)
{
	std::vector<std::string>::iterator	it;

	std::cout << "\nRequest Header : " << std::endl;
	for (it = header_tab.begin(); it != header_tab.end(); it++)
		std::cout << *it.base() << std::endl;
}
*/

// Give the a stream containing the raw request from the client and an empty string
// object. The dummy response will be put in this empty string object.
std::string&	get_dummy_response(ServerHTTP &srv, std::stringstream& raw_request,
	std::string& response)
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
	std::cout << response << std::endl;
	return (response);
}

// Gathers next request received through pipe and 
int	WebservRequestAgent::process_incoming_request()
{
	
}