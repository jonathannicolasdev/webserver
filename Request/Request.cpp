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

Request::Request(const std::string &raw_request): //_method(NULL_M), 
	header_is_parsed(false), header_offset(0), content_length(0), is_multipart(false)
{
	std::cout << "Request Constructor" << std::endl;
	this->_raw_request = raw_request;

}


Request::Request(void): //_method(NULL_M),
	header_is_parsed(false), header_offset(0), content_length(0), is_multipart(false)
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
/*
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
		return (Logger::log(LOG_WARNING, std::string("Received request with invalid method") + method));

	_method_str = method;

	if ((p1 = this->path.find('?')) != this->path.npos)
	{
		printf("FOUND ? IN PATH !!\n");
		this->query = this->path.substr(p1);
		this->path = this->path.substr(0, p1);
	}

	// DEBUG
	// std::ostringstream ss, ss2, ss3, ss4;
	// std::cout << "Split request line info : " << std::endl;
	// ss << method.length();
	// std::cout << " - method : " << method << " (length : " << ss.str() << ")" << std::endl;
	// ss2 << this->path.length();
	// std::cout << " - path : " << this->path << " (length : " << ss2.str() << ")" << std::endl;
	// ss3 << this->query.length();
	// std::cout << " - query : " << this->query << " (length : " << ss3.str() << ")" << std::endl;
	// ss4 << this->protocol.length();
	// std::cout << " - proto : " << this->protocol << " (length : " << ss4.str() << ")" << std::endl;
	// std::cout << "rest of the request : " << std::endl;
	// std::cout << this->_raw_request << std::endl;
	// DEBUG END

	return (0);
}
*/

bool Request::getMultiformFlag(void) const {return (this->is_multipart);}
//bool Request::getMultiformFlag(void) const {return (true);}

bool Request::processMultiform(void) 
{ 
	std::map<std::string, std::string>::const_iterator it;

	std::cout << "\n\nprocessMultiform : " << std::endl;

	it = this->header.find("Content-Type");
	if (it == this->header.end())
		return (false);
	
	std::cout << "processMultiform : Content-Type : " << (*this)["Content-Type"] << std::endl;

	std::cout << "multipart index : " <<  it->second.find("multipart/form-data") << std::endl;
	if (it->second.find("multipart/form-data") != std::string::npos)
	{
		std::cout << "multipart/form-data FOUND !!" << std::endl;
		this->is_multipart = true;
		size_t	pos = it->second.find("boundary=");
		if (pos == std::string::npos)
		{
			std::cout << "NO MULTIPART" << std::endl;
			return (false);
		}
		pos += 9;
		this->boundary = std::string("--") + it->second.substr(pos);
		
		return (true);
	}

	// for (it = Request::header.begin(); it != Request::header.end(); ++it)
	// {
	// 	if (it->first == "Content-Type")
	// 	{
	// 		int pos = it->second.find("multipart/form-data", 0);
	// 		if (pos != std::string::npos)
	// 		{
	// 			return true;
	// 		}
	// 	}
	// }
	return false;
 }

// splits header passed and puts key, value pairs in header map.
int Request::process_header(void) // const std::string& raw_header)
{
//	size_t line_start, column_pos, line_end;
//	std::string key, value;
//	bool stop = false;
//	std::ostringstream	os;

//	std::cout << "Request::process_header() START ATTEMPT : " << std::endl;
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
	std::cout << "Request HEADER : " << std::endl;
	std::cout << this->_raw_header << std::endl;
	split_string(this->_raw_header, "\r\n", split_header);
	if (split_header.size() == 0)
		return (Logger::log(LOG_WARNING, std::string("Received request, with invalid header : ") + this->_raw_header), -1);
	split_string(split_header[0], ' ', split_line);
	if (split_line.size() < 3)
		return (Logger::log(LOG_WARNING, std::string("Received request, but request line is invalid : ") + split_header[0]), -1);

	///	Processing request line. (first in split_header)
	this->_method_str = split_line[0];//*(it++);
	this->path = split_line[1];//*(it++);
	this->protocol = split_line[2];//*(it++);
	if ((pos = this->path.find('?')) != std::string::npos)
		this->query = this->path.substr(pos);

	for (it = (split_header.begin() + 1); it != split_header.end(); ++it)
	{
		//split_line.clear();
		//split_string(*it, ':');
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
/*	
//	std::cout << "Request::process_header() START : " << std::endl;
	line_start = 0;
	while (!stop && this->_raw_request[line_start] && !isspace(this->_raw_request[line_start]))
	{
		column_pos = this->_raw_request.find(':', line_start);
		line_end = this->_raw_request.find_first_of("\r\n", column_pos);

		key = this->_raw_request.substr(line_start, column_pos - line_start);
		column_pos = this->_raw_request.find_first_not_of(": ", column_pos);
		value = this->_raw_request.substr(column_pos, line_end - column_pos);

		// DEBUG
//		std::cout << "key : " << key << ", value : " << value << std::endl;
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
*/
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
	if (this->body.length() == 0)
		std::cout << "THERE IS NO BODY !!" << std::endl;
	else
		processMultiform();

	std::cout << "Request::process_body() END" << std::endl;
	return (0);
}

/*
const std::string&	Request::getBoundary() const
{
	//  -H "Content-Type: multipart/form-data; boundary=boundary123456789" \

	std::map<std::string, std::string>::const_iterator it;

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
*/
int Request::process_raw_request(void) // const std::string& raw_request)
{
	//if (this->process_request_line() < 0 || this->process_header() < 0 || this->process_body() < 0)
	if (this->process_header() < 0 || this->process_body() < 0)
		return (-1);
	
	// std::cout << std::endl << "Request::process_raw_request : " << std::endl;
	// std::map<std::string, std::string>::iterator	it;
	// for (it = header.begin(); it != header.end(); ++it)
	// {
	// 	std::cout << "\"" << it->first << "\": " << it->second << std::endl;
	// }
	
	return (0);
}

const std::string &
Request::get_method(void) const { return (this->_method_str); }

//bool Request::is_method(enum e_method method) const { return (this->_method == method); }

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

size_t
Request::get_header_length(void) const {return (this->header_offset);}

/*
{
	this->_raw_request += req_buff;
	std::cout << "vvvvvvvvvvvvvv " << this->_raw_request.length() << std::endl;

	return (*this);
}
*/

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
//	std::vector<DataPart> dataparts;
	std::string datapart_str;
	size_t startPos = 0;
	size_t endPos;

	std::cout <<  "\n\nExtract_multipart strs : " << std::endl;

//	std::cout << body << std::endl;
//	std::cout << "boundary.length() :" << boundary.length() << std::endl;

	startPos = boundary.length();
	std::cout << "first startPos : " << startPos << std::endl;

//	std::cout << "ooooooooooooooooooo  starting extract:" <<std::endl;

	while ((endPos = body.find(boundary, startPos)) != std::string::npos && (endPos != startPos))
	{
//		std::cout << "ooooooooooooooooooo  part found" << std::endl;

		startPos = body.find_first_not_of("\r\n", startPos);
	//	if (endPos != 0)
	//	{
//		std::cout << "startPos : " << startPos << ", endPos : " << endPos << std::endl;
		datapart_str = body.substr(startPos, endPos - startPos);

//		std::cout << "newline index : " << datapart_str.find("\r\n") << std::endl;
//		std::cout << "datapart_str : " << datapart_str << std::endl;
		dataparts.push_back(DataPart(datapart_str));
		//	std::cout << dataparts[dataparts.size() - 1] << std::endl;
	//	}
//		std::cout << "old startPos : " << startPos << std::endl;
		startPos = endPos + boundary.length();
//		startPos += datapart_str.length();
//		startPos = body.find_first_not_of("\r\n", startPos);
//		std::cout << "new startPos : " << startPos << std::endl;
		//startPos = endPos + boundary.length();
	}
//	return dataparts;
}

//std::ostream &operator<<(std::ostream &os, const Request &req)
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
			char	*end_ptr = NULL;
			content_length = std::strtol(it->second.c_str(), &end_ptr, 10);
			//content_length = std::stol(it->second);
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
