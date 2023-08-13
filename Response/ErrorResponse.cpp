/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorResponse.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:42:23 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/09 20:35:02 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorResponse.hpp"

//std::map<int, std::string> ErrorResponse::status_msgs = {
const std::map<int, std::string> status_msgs_init(void)
{
	std::map<int, std::string>	map;

	// 1xx status messages, INFORMATIONAL
	map[100] = "100 Continue";
	map[101] = "101 Switching Protocols";

	// 2xx status messages, SUCCESS
	map[200] = "200 OK";
	map[201] = "201 Created";
	map[202] = "202 Accepted";
	map[203] = "203 Non-Authoritative Information";
	map[204] = "204 No Content";
	map[205] = "205 Reset Content";

	// 3xx status messages, REDIRECTIONS
	map[300] = "300 Multiple Choices";
	map[301] = "301 Moved Permanently";
	map[302] = "302 Found";
	map[303] = "303 See Other";
	map[305] = "305 Use Proxy";
	map[307] = "307 Temporary Redirect";

	// 4xx status messages, CLIENT ERR;
	map[400] = "400 Bad Request";
	map[402] = "402 Payment Required";
	map[403] = "403 Forbidden";
	map[404] = "404 Not Found";
	map[405] = "405 Method Not Allowed";
	map[406] = "406 Not Acceptable";
	map[408] = "408 Request Timeout";
	map[409] = "409 Conflict";
	map[410] = "410 Gone";
	map[411] = "411 Length Required";
	map[413] = "413 Payload Too Large";
	map[414] = "414 URI Too Long";
	map[415] = "415 Unsupported Media Type";
	map[417] = "417 Expectation Failed";
	map[426] = "426 Upgrade Required";

	// 5xx status messages, SERVER ERR;
	map[500] = "500 Internal Server Error";
	map[501] = "501 Not Implemented";
	map[502] = "502 Bad Gateway";
	map[503] = "503 Service Unavailable";
	map[504] = "504 Gateway Timeout";
	map[505] = "505 HTTP Version Not Supported";
	return (map);
}

const std::map<int, std::string> ErrorResponse::status_msgs = status_msgs_init();


//ErrorResponse::ErrorResponse(void) {}
ErrorResponse::ErrorResponse(const ServerHTTP& srv, const Request& req, const ServerConfig& cfg):
	_srv(srv), _req(req), _cfg(cfg)
{
	(void)_srv;
	(void)_req;
	(void)_cfg;
}

ErrorResponse::~ErrorResponse(void) {}


bool	_get_status_message(int error_code, std::string& ret_string)
{
	std::map<int, std::string>::const_iterator	it;
	const std::string		status_msg;

	if ((it = ErrorResponse::status_msgs.find(error_code)) == ErrorResponse::status_msgs.end())
		return (false);

	ret_string = it->second;
	return (true);
}

bool	ErrorResponse::__prepare_error(const std::string& dir_path, int error_code)
{
	std::ostringstream	err_code_str;
	std::stringstream	file_buff;
	std::ifstream		fs;
	std::string			filepath = dir_path;
	std::string			status_msg;
	std::string			header;
	std::string			body;
	std::ostringstream	content_length;
//	std::string			line;//ErrorResponse;
//	const std::map<int, std::string>&	cfg_error_pages = _cfg.GetError_pages();
//	std::map<int, std::string>::iterator	epage;
	if (filepath[filepath.length() - 1] != '/')
		filepath += '/';
	err_code_str << error_code;
	filepath += err_code_str.str() + ".html";
	std::cout << "error code : " << error_code << ", filepath : " << filepath << std::endl;
	
//	fs.open(_cfg.GetRootDir(), std::io::in);
	//fs.open("default_errors/404.html");// This should the last resort. Search for the config error pages first.
	fs.open(filepath.c_str());// This should the last resort. Search for the config error pages first.
	if (!fs.is_open()
		|| !_get_status_message(error_code, status_msg))
	{
		std::cerr << "is file open : " << fs.is_open() << ", status msg : " << status_msg << std::endl;
		_prepare_default_hardcoded_500_error();
		return (false);
	}
	std::cout << "error code: " << error_code << ", status msg : " << status_msg << std::endl;

	header = "HTTP/1.1 ";//"404 OK\r\n";
	header += status_msg + "\r\n";
	header += "Content-type: text/html; charset=utf-8\r\n";

	file_buff << fs.rdbuf();
//	body += file_buff.str();

//	std::cout << body;
//	content_length << body.length();
	//Find content_length
	file_buff.seekp(0, std::ios::end);
	content_length << file_buff.tellp();
	file_buff.seekp(0, std::ios::beg);
	std::cout << "Error Page Content Length : " << content_length.str() << std::endl;
//	body += file_buff.str();
//	while (std::getline(fs, line))
//		body += line;

	header += "Content-length: ";
//	content_length << body.length();
	header += content_length.str();
	header += "\r\n\r\n";
	
	this->_text = header + file_buff.str();//body;
	return (true);
}


/// Add more as needed or just call __prepare_error directly.
bool	ErrorResponse::_prepare_default_404_error(void) {return (__prepare_error("default_errors/", 404));}
bool	ErrorResponse::_prepare_default_500_error(void) {return (__prepare_error("default_errors/", 500));}


/// REQUIRED METHODS BY SERVER ///////////////
int ErrorResponse::prepare_response(int error_code)
{
	std::cout << "Preparing ErrorResponse." << std::endl;

	__prepare_error("default_errors/", error_code);
	return (0);
}

//const std::string&	ErrorResponse::get_response(void) const {return (this->_text);}
/// END OF REQUIRED METHODS BY SERVER /////////////

void    ErrorResponse::_prepare_default_hardcoded_500_error(void)
{
	std::string			header;
	std::string			body;
	std::ostringstream	content_length;
//	std::string			ErrorResponse;

	header = "HTTP/1.1 500 Internal Server Error\r\n";
	header += "Content-type: text/html\r\n";

	body += "<!DOCTYPE html>\r\n";
	body += "<html lang=\"en\">\r\n";
	body += "<head>\r\n";
	body += "	<meta charset=\"UTF-8\">\r\n";
	body += "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
	body += "	<title>500 Internal Error</title>\r\n";
	body += "	<style>\r\n";
	body += "		body {\r\n";
	body += "			font-family: Arial, sans-serif;\r\n";
	body += "			text-align: center;\r\n";
	body += "			margin: 0;\r\n";
	body += "			padding: 0;\r\n";
	body += "			background-color: #f4f4f4;\r\n";
	body += "		}\r\n";
	body += "		.container {\r\n";
	body += "			padding: 100px;\r\n";
	body += "		}\r\n";
	body += "		h1 {\r\n";
	body += "			font-size: 72px;\r\n";
	body += "			margin-bottom: 20px;\r\n";
	body += "		}\r\n";
	body += "		p {\r\n";
	body += "			font-size: 18px;\r\n";
	body += "			margin-bottom: 30px;\r\n";
	body += "		}\r\n";
	body += "		a {\r\n";
	body += "			color: #333;\r\n";
	body += "			text-decoration: none;\r\n";
	body += "			font-weight: bold;\r\n";
	body += "		}\r\n";
	body += "	</style>\r\n";
	body += "</head>\r\n";
	body += "<body>\r\n";
	body += "	<div class=\"container\">\r\n";
	body += "		<h1>500</h1>\r\n";
	body += "		<p>Oops! An internal error occured while treating your request. Try again later.</p>\r\n";
	body += "		<p>Return to <a href=\"/\">home</a>.</p>\r\n";
	body += "	</div>\r\n";
	body += "</body>\r\n";
	body += "</html>\r\n";


	header += "Content-length: ";
	content_length << body.length();
	header += content_length.str();
	header += "\r\n\r\n";
	this->_text = header + body;
}
