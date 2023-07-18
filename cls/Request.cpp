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

Request::Request(const std::string& raw_request): _raw_request(raw_request), _method(NULL_M)
{
	std::cout << "Request Constructor" << std::endl;

}

Request::~Request()
{
	std::cout << "Request Destructor" << std::endl;
}

// splits header passed and puts key, value pairs in header map.
int
Request::process_header(const std::string& raw_header)
{

}

int
Request::process_raw_request(const std::string& raw_request)
{
	size_t	pos;

	pos = raw_request.find("\n\r\n\r\n");
	if (pos == raw_request.npos)
	{
		// Process only header
		this->_raw_header = raw_request;
	}
	else
	{
		this->header = raw_request.substr(0, pos);
		this->body = raw_request.substr(pos + 5);
		// Process header + body

	}
}

enum e_method
Request::get_method(void) const {return (this->_method);}

bool
Request::is_method(enum e_method method) const {return (this->_method == method);}

const std::string*	Request::operator[](const std::string& key) const
{
	std::map<std::string, std::string>::iterator	it;
	
	it = this->header.find(key);
	if (it == this->header.end())
		return (NULL);
	return (&it->second);
}
