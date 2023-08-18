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

Request::Request(const std::string &raw_request) : _method(NULL_M)
{
	std::cout << "Request Constructor" << std::endl;
	this->_raw_request = raw_request;
}

Request::Request(void) : _method(NULL_M)
{
	std::cout << "Request default constructor" << std::endl;
}

Request::~Request()
{
	std::cout << "Request Destructor" << std::endl;
}

const std::string &Request::get_path() const
{
	return path;
}

const std::string &Request::get_body() const
{
	return body;
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
	this->_raw_request = this->_raw_request.substr(this->_raw_request.find_first_not_of("\r\n", line_end));

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
	std::cout << "rest of the request : " << std::endl;
	std::cout << this->_raw_request << std::endl;
	// DEBUG END

	return (0);
}

// splits header passed and puts key, value pairs in header map.
int Request::process_header(void) // const std::string& raw_header)
{
	size_t line_start, column_pos, line_end;
	std::string key, value;
	bool stop = 0;

	std::cout << "Request::process_header() : " << std::endl;
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
	if (this->_raw_request[line_start])
		this->body = this->_raw_request.substr(line_start);
	return (0);
}

int Request::process_body(void)
{
	// TODO ... or not
	return (0);
}

int Request::process_raw_request(void) // const std::string& raw_request)
{
	if (this->process_request_line() < 0 || this->process_header() < 0 || this->process_body() < 0)
		return (-1);
	/*
	size_t	pos;

	if (this->is_empty())
		return (Logger::log(LOG_WARNING, "Trying to process raw request but request is empty"));
//	pos = raw_request.find("\n\r\n\r\n");
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
	*/
	return (0);
}

const std::string &
Request::get_method(void) const { return (this->_method_str); }

bool Request::is_method(enum e_method method) const { return (this->_method == method); }

const std::string &Request::get_raw_request(void) const { return (_raw_request); }

bool Request::getMultiformFlag() const 
{ 
	
	std::map<std::string, std::string>::iterator it;

	for (it = Request::header.begin(); it != Request::header.end(); ++it)
	{
		if (it->first == "Content-Type")
		{
			int pos = it->second.find("multipart/form-data", 0);
			if (pos != std::string::npos)
			{
				return true;
			}
		}
	}
	return false;
 }


std::string Request::getBoundary() const
{
	//  -H "Content-Type: multipart/form-data; boundary=boundary123456789" \

	std::map<std::string, std::string>::iterator it;

	for (it = Request::header.begin(); it != Request::header.end(); ++it)
	{
		if (it->first == "Content-Type")
		{
			int pos = it->second.find("boundary=", 0);
			if (pos != std::string::npos)
			{
				return it->second.substr(pos, it->second.length() - pos);
			}
		}
	}
	return "";
}

const std::string &Request::operator[](const std::string &key) const
{
	static const std::string nullstr = "";
	std::map<std::string, std::string>::const_iterator it;

	it = this->header.find(key);
	if (it == this->header.end())
		return (nullstr);
	// return (&nullstr);
	return (it->second);
	// return (&it->second);
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

Request &
Request::operator<<(char *req_buff)
{
	this->_raw_request += req_buff;
	return (*this);
}

std::string removeBoundary(std::string &body, std::string &boundary)
{
	std::string buffer;
	std::string new_body;
	std::string filename;
	bool is_boundary = false;
	bool is_content = false;

	if (body.find("--" + boundary) != std::string::npos && body.find("--" + boundary + "--") != std::string::npos)
	{
		for (size_t i = 0; i < body.size(); i++)
		{
			buffer.clear();
			while (body[i] != '\n')
			{
				buffer += body[i];
				i++;
			}
			if (!buffer.compare(("--" + boundary + "--\r")))
			{
				is_content = true;
				is_boundary = false;
			}
			if (!buffer.compare(("--" + boundary + "\r")))
			{
				is_boundary = true;
			}
			if (is_boundary)
			{
				if (!buffer.compare(0, 31, "Content-Disposition: form-data;"))
				{
					size_t start = buffer.find("filename=\"");
					if (start != std::string::npos)
					{
						size_t end = buffer.find("\"", start + 10);
						if (end != std::string::npos)
							filename = buffer.substr(start + 10, end);
					}
				}
				else if (!buffer.compare(0, 1, "\r") && !filename.empty())
				{
					is_boundary = false;
					is_content = true;
				}
			}
			else if (is_content)
			{
				if (!buffer.compare(("--" + boundary + "\r")))
				{
					is_boundary = true;
				}
				else if (!buffer.compare(("--" + boundary + "--\r")))
				{
					new_body.erase(new_body.end() - 1);
					break;
				}
				else
					new_body += (buffer + "\n");
			}
		}
	}

	body.clear();
	return (new_body);
}
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

std::vector<DataPart> Request::extract_multipart() const
{
	std::vector<DataPart> dataparts;
	std::string body = get_body();
	std::string boundary = "--" + getBoundary();
	std::string datapart_str;
	size_t startPos = 0;
	size_t endPos;

	while ((endPos = body.find(boundary, startPos)) != std::string::npos)
	{
		if (endPos != 0)
		{
			datapart_str = body.substr(startPos, endPos - startPos);
			dataparts.push_back(DataPart(datapart_str));
		}
		startPos = endPos + boundary.length();
	}

	return dataparts;
}

std::ostream &operator<<(std::ostream &os, const Request &req)
{
	os << req.get_raw_request();
	return (os);
}
