/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 22:27:19 by marvin            #+#    #+#             */
/*   Updated: 2023/07/17 22:27:19 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const std::string &raw_request) : _method(NULL_M), 
	header_is_parsed(false), header_offset(0), content_length(0)
{
	std::cout << "Request Constructor" << std::endl;
	this->_raw_request = raw_request;
}

Request::Request(void) : _method(NULL_M), 
	header_is_parsed(false), header_offset(0), content_length(0)
{
	std::cout << "Request default constructor" << std::endl;
}

Request::~Request()
{
	std::cout << "Request Destructor" << std::endl;
}

const std::string& Request::get_path() const
{
	return path;
}

int Request::process_request_line(void)
{
	size_t line_end, p1, p2;
	std::string method;
	//	std::string	path;
	//	std::string	proto;
	// std::getline(this->_raw_request, this->_raw_request_line, '\n');
	line_end = this->_raw_request.find_first_of("\r\n");
	this->_raw_request_line = this->_raw_request.substr(0, line_end);
	//	this->_raw_request = this->_raw_request.substr(line_end + this->_raw_request_line);
//	this->_raw_request = this->_raw_request.substr(this->_raw_request.find_first_not_of("\r\n", line_end));

	p1 = this->_raw_request_line.find(' ');
	p2 = this->_raw_request_line.find_last_of(" ");

	method = this->_raw_request_line.substr(0, p1);
	this->path = this->_raw_request_line.substr(p1 + 1, p2 - p1 - 1);
	this->protocol = this->_raw_request_line.substr(p2 + 1);

	if (method == "GET")
		this->_method = GET_M;
	else if (method == "POST")
		this->_method = POST_M;
	else if (method == "DELETE")
		this->_method = POST_M;
	else if (method == "PUT")
		this->_method = POST_M;
	else
		return (Logger::log(LOG_WARNING, "Received request with invalid method"));

	_method_str = method;

	if ((p1 = this->path.find('?')) != this->path.npos)
	{
		printf("FOUND ? IN PATH !!\n");
		this->query = this->path.substr(p1);
		this->path = this->path.substr(0, p1);
	}

	// DEBUG
	std::ostringstream ss, ss2, ss3, ss4;
	std::cout << "Split request line info : " << std::endl;
	ss << method.length();
	std::cout << " - method : " << method << " (length : " << ss.str() << ")" << std::endl;
	ss2 << this->path.length();
	std::cout << " - path : " << this->path << " (length : " << ss2.str() << ")" << std::endl;
	ss3 << this->query.length();
	std::cout << " - query : " << this->query << " (length : " << ss3.str() << ")" << std::endl;
	ss4 << this->protocol.length();
	std::cout << " - proto : " << this->protocol << " (length : " << ss4.str() << ")" << std::endl;
//	std::cout << "rest of the request : " << std::endl;
//	std::cout << this->_raw_request << std::endl;
	// DEBUG END

	return (0);
}

// splits header passed and puts key, value pairs in header map.
int Request::process_header(void) // const std::string& raw_header)
{
	size_t line_start, column_pos, line_end;
	std::string key, value;
	bool stop = false;
//	std::ostringstream	os;

	std::cout << "Request::process_header() START ATTEMPT : " << std::endl;

	if (this->header_is_parsed)
		return (0);

	std::cout << "Request::process_header() START : " << std::endl;
	line_start = 0;
	while (!stop && this->_raw_request[line_start] && !isspace(this->_raw_request[line_start]))
	{
		column_pos = this->_raw_request.find(':', line_start);
		line_end = this->_raw_request.find_first_of("\r\n", column_pos);

		key = this->_raw_request.substr(line_start, column_pos - line_start);
		column_pos = this->_raw_request.find_first_not_of(": ", column_pos);
		value = this->_raw_request.substr(column_pos, line_end - column_pos);

		// DEBUG
		std::cout << "key : " << key << ", value : " << value << std::endl;
		// DEBUG END

		this->header[key] = value;
		line_start = line_end;
		stop = (this->_raw_request.compare(line_start, 4, "\r\n\r\n") == 0);
		while (this->_raw_request[line_start] && isspace(this->_raw_request[line_start]))
			line_start++;
	}
//	if (this->_raw_request[line_start])
//	{
//		this->body = this->_raw_request.substr(line_start);
//		os << this->body.length();
//		this->content_length_str = os.str();
//		this->content_length = this->body.length();;
//	}
	std::cout << "Request::process_header() EXIT " << std::endl;
	header_is_parsed = true;
	std::cout << "Request::process_header() header is parsed " << std::endl;
	return (0);
}

int Request::process_body(void)
{
	// TODO ... or not
	std::cout << "Request::process_body() START" << std::endl;
	std::cout << "header_offset vs _raw_request length : " << header_offset << " vs " << _raw_request.length() << std::endl;
	if (header_offset != _raw_request.length())
		this->body = _raw_request.substr(header_offset);
	std::cout << "Request::process_body() END" << std::endl;
	return (0);
}

int Request::process_raw_request(void) // const std::string& raw_request)
{
	if (this->process_request_line() < 0 || this->process_header() < 0 || this->process_body() < 0)
		return (-1);
	return (0);
}



const std::string&
Request::get_method(void) const { return (this->_method_str); }

bool Request::is_method(enum e_method method) const { return (this->_method == method); }

const std::string&	Request::get_raw_request(void) const {return (_raw_request);}
const std::string&	Request::get_query(void) const {return (query);}

size_t				Request::get_content_length(void) const {return (this->content_length);}
const std::string&	Request::get_content_length_str(void) const {return (this->content_length_str);}


const std::string&	Request::operator[](const std::string& key) const
{
	static const std::string nullstr = "";
	std::map<std::string, std::string>::const_iterator it;

	it = this->header.find(key);
	if (it == this->header.end())
		return (nullstr);
		//return (&nullstr);
	return (it->second);
	//return (&it->second);
}

bool Request::is_empty(void)
{
	return (this->_raw_request.empty());
}

size_t
Request::length(void) const
{
	return (this->_raw_request.length());
}

size_t
Request::get_header_length(void) const {return (this->header_offset);}

Request&
//Request::operator<<(char *req_buff)
Request::append(const char *req_buff, size_t n)
{
	size_t	init_len = _raw_request.length();
	std::map<std::string, std::string>::iterator	it;

	this->_raw_request.append(req_buff, n);// += req_buff;
	if (!header_is_parsed
		&& (header_offset = this->_raw_request.find("\r\n\r\n", init_len)) != std::string::npos)
	{
		header_offset += 4;
		process_header();
		std::cout << "header_offset : " << header_offset << std::endl;
		it = header.find("Content-Length");
		std::cout << "it found ? : " << (it != header.end()) << std::endl;
		if (it != header.end())
		{
			content_length_str = it->second;
			content_length = std::stol(it->second);
			std::cout << "content length : " << content_length_str << ", vs stol content length : " << content_length << std::endl;
		}
	}
	return (*this);
}

bool
Request::is_header_parsed(void) const {return (this->header_is_parsed);}

std::ostream&			operator<<(std::ostream& os, const Request& req)
{
	os << req.get_raw_request();
	return (os);
}
