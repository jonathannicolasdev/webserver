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

Request::Request(const std::string& raw_request): _method(NULL_M), _raw_request(raw_request)
{
	std::cout << "Request Constructor" << std::endl;
}

Request::Request(void): _method(NULL_M)
{
	std::cout << "Request default constructor" << std::endl;
}

Request::~Request()
{
	std::cout << "Request Destructor" << std::endl;
}

// splits header passed and puts key, value pairs in header map.
int
Request::process_header(const std::string& raw_header)
{
	UNUSED(raw_header);
	return (0);
}

int
Request::process_raw_request(const std::string& raw_request)
{
	size_t	pos;

	if (raw_request.empty())
		return (Logger::log(LOG_WARNING, "Trying to process raw request but request is empty"));
	pos = raw_request.find("\n\r\n\r\n");
	if (pos == raw_request.npos)
	{
		// Process only header
		this->_raw_header = raw_request;
	}
	else
	{
		this->_raw_header = raw_request.substr(0, pos);
		this->body = raw_request.substr(pos + 5);
		// Process header + body

	}
	return (0);
}

enum e_method
Request::get_method(void) const {return (this->_method);}

bool
Request::is_method(enum e_method method) const {return (this->_method == method);}

const std::string*	Request::operator[](const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator	it;
	
	it = this->header.find(key);
	if (it == this->header.end())
		return (NULL);
	return (&it->second);
}
