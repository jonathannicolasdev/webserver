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

	if ((it = status_msgs.find(error_code)) == status_msgs.end())
		return (false);

	ret_string = it->second;
	return (true);
}

void	ErrorResponse::__prepare_error(const std::string& filepath, int error_code)
{
	std::string			status_msg, header;
	std::ifstream		fs;
	std::ostringstream	text_buff;
	
	if (!_get_status_message(error_code, status_msg))
	{
		_prepare_default_hardcoded_500_error();
		return ;
	}
	header = "HTTP/1.1 ";
	header += status_msg + "\r\n";
	header += "Content-type: text/html; charset=utf-8\r\n";

	fs.open(filepath.c_str());
	if (!fs.is_open())
	{
		_prepare_default_hardcoded_500_error();
		return ;
	}
	text_buff << "Content-length: ";
	fs.seekg(0, std::ios::end);
	text_buff << fs.tellg();
	fs.seekg(0, std::ios::beg);
	text_buff << "\r\n\r\n";
	text_buff << fs.rdbuf();
	fs.close();

	this->_text = header + text_buff.str();
}

static int	_get_default_error_path(int error_code, std::string& ret_path)
{
	std::ostringstream				os;
	std::string						filepath;
	std::vector<std::string>		path_segments;

	path_segments.push_back(ServerConfig::cwd.c_str());
	os << error_code;
	path_segments.push_back("default_errors");
	filepath = os.str() + ".html";
	path_segments.push_back(filepath);
	join_strings(path_segments, '/', ret_path);
	return (access(ret_path.c_str(), F_OK | R_OK));
}

static int	_get_custom_error_path(const ServerConfig& cfg, const std::string& cfg_errpath, std::string& ret_path)
{
	std::ostringstream				os;
	std::string						filepath;
	std::vector<std::string>		custom_path_segments;
	std::vector<std::string>		path_segments;

	split_string(cfg_errpath, '/', custom_path_segments);
	path_segments.push_back(ServerConfig::cwd.c_str());
	path_segments.insert(path_segments.end(), cfg.GetSplitRoot().begin(), cfg.GetSplitRoot().end());
	path_segments.insert(path_segments.end(),
		custom_path_segments.begin(), custom_path_segments.end());
	join_strings(path_segments, '/', ret_path);
	return (access(ret_path.c_str(), F_OK | R_OK));
}

/// REQUIRED METHODS BY SERVER ///////////////
void	ErrorResponse::prepare_response(int error_code)
{
	std::map<int, std::string>		error_pages = this->_cfg.GetError_pages();
	std::map<int, std::string>::iterator	it;
	std::string						filepath;
	std::vector<std::string>		path_segments;

	path_segments.push_back(ServerConfig::cwd.c_str());

	it = error_pages.find(error_code);
	if (it == error_pages.end())
	{
		if (_get_default_error_path(error_code, filepath) == 0)
			__prepare_error(filepath, error_code);
		else 
			_prepare_default_hardcoded_500_error();
	}
	else
	{
		if (_get_custom_error_path(_cfg, it->second, filepath) == 0)
			__prepare_error(filepath, error_code);
		else if (_get_default_error_path(error_code, filepath) == 0)
			__prepare_error(filepath, error_code);
		else
			_prepare_default_hardcoded_500_error();
	}
}

void    ErrorResponse::_prepare_default_hardcoded_500_error(void)
{
	std::string			header;
	std::string			body;
	std::ostringstream	content_length;

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
