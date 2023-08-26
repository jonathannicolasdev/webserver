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

Request::Request(const std::string &raw_request):
	header_is_parsed(false), header_offset(0), content_length(0), is_multipart(false)
{
	this->_raw_request = raw_request;

}

Request::Request(void):
	header_is_parsed(false), header_offset(0), content_length(0), is_multipart(false)
{
}

Request::~Request()
{
}

const std::string &Request::get_path() const
{
	return path;
}

const std::string &Request::get_body() const
{
	return body;
}

bool Request::getMultiformFlag(void) const {return (this->is_multipart);}

bool Request::processMultiform(void) 
{ 
	std::map<std::string, std::string>::const_iterator it;

	it = this->header.find("Content-Type");
	if (it == this->header.end())
		return (false);
	
	if (it->second.find("multipart/form-data") != std::string::npos)
	{
		this->is_multipart = true;
		size_t	pos = it->second.find("boundary=");
		if (pos == std::string::npos)
		{
			std::cerr << "NO MULTIPART" << std::endl;
			return (false);
		}
		pos += 9;
		this->boundary = std::string("--") + it->second.substr(pos);
		
		return (true);
	}
	return false;
 }

// splits header passed and puts key, value pairs in header map.
int Request::process_header(void)
{
	if (this->header_is_parsed)
		return (0);

	size_t	pos, pos_end;
	std::vector<std::string>			split_header;
	std::vector<std::string>			split_line;
	std::vector<std::string>::iterator	it;
	std::string		key, value;

	pos_end = this->_raw_request.find("\r\n\r\n");
	if (pos_end == std::string::npos)
		return (Logger::log(LOG_WARNING, "Received request, but no header delimiter found"), -1);
	this->_raw_header = this->_raw_request.substr(0, pos_end);
	split_string(this->_raw_header, "\r\n", split_header);
	if (split_header.size() == 0)
		return (Logger::log(LOG_WARNING, std::string("Received request, with invalid header : ") + this->_raw_header), -1);
	split_string(split_header[0], ' ', split_line);
	if (split_line.size() < 3)
		return (Logger::log(LOG_WARNING, std::string("Received request, but request line is invalid : ") + split_header[0]), -1);

	///	Processing request line. (first in split_header)
	this->_method_str = split_line[0];
	this->path = split_line[1];
	this->protocol = split_line[2];
	if ((pos = this->path.find('?')) != std::string::npos)
		this->query = this->path.substr(pos);

	for (it = (split_header.begin() + 1); it != split_header.end(); ++it)
	{
		pos = (*it).find(':');
		if (pos == std::string::npos)
		{
			Logger::log(LOG_WARNING, std::string("Request skipping invalid header tag : ") + *it);
			continue ;
		}
		key = (*it).substr(0, pos);
		value = (*it).substr(pos + 2);
		this->header[key] = value;
	}
	header_is_parsed = true;
	return (0);
}

int Request::process_body(void)
{
	if (header_offset != _raw_request.length())
		this->body = _raw_request.substr(header_offset);
	if (this->body.length() != 0)
		processMultiform();
//		std::cout << "THERE IS NO BODY !!" << std::endl;
//	else

	return (0);
}

int Request::process_raw_request(void)
{
	if (this->process_header() < 0 || this->process_body() < 0)
		return (-1);
	return (0);
}

const std::string &
Request::get_method(void) const { return (this->_method_str); }

const std::string&	Request::get_raw_request(void) const {return (_raw_request);}
const std::string&	Request::get_query(void) const {return (query);}

size_t				Request::get_content_length(void) const {return (this->content_length);}
const std::string&	Request::get_content_length_str(void) const {return (this->content_length_str);}


const std::string &Request::operator[](const std::string &key) const
{
	static const std::string nullstr = "";
	std::map<std::string, std::string>::const_iterator it;

	it = this->header.find(key);
	if (it == this->header.end())
		return (nullstr);
	return (it->second);
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

std::vector<std::string> extractContents(const std::string &body, const std::string &boundary)
{
	std::vector<std::string> contents;

	size_t startPos = 0;
	size_t endPos;

	while ((endPos = body.find(boundary, startPos)) != std::string::npos)
	{
		if (endPos != 0)
			contents.push_back(body.substr(startPos, endPos - startPos));
		startPos = endPos + boundary.length();
	}

	return contents;
}

// Assums dataparts / boundery and content all are present and valide in the request body.
void Request::extract_multipart(std::vector<DataPart>& dataparts) const
{
	std::string datapart_str;
	size_t startPos = 0;
	size_t endPos;


	startPos = boundary.length();

	while ((endPos = body.find(boundary, startPos)) != std::string::npos && (endPos != startPos))
	{
		startPos = body.find_first_not_of("\r\n", startPos);
		datapart_str = body.substr(startPos, endPos - startPos);
		dataparts.push_back(DataPart(datapart_str));
		startPos = endPos + boundary.length();
	}
}

Request&
Request::append(const char *req_buff, size_t n)
{
	size_t	init_len = _raw_request.length();
	std::map<std::string, std::string>::iterator	it;

	this->_raw_request.append(req_buff, n);
	if (!header_is_parsed
		&& (header_offset = this->_raw_request.find("\r\n\r\n", init_len)) != std::string::npos)
	{
		header_offset += 4;
		process_header();
		it = header.find("Content-Length");
		if (it != header.end())
		{
			content_length_str = it->second;
			char	*end_ptr = NULL;
			content_length = std::strtol(it->second.c_str(), &end_ptr, 10);
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
