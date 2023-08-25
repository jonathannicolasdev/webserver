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
	
	std::cout << "__prepare_error start with filepath : " << filepath << ", error_code : " << error_code << std::endl;

//	if (access(filepath.c_str(), F_OK | R_OK) || !_get_status_message(error_code, status_msg))
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
//		std::cerr << "failed to open error page file" << std::endl;
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
/*
bool	ErrorResponse::__prepare_error(const std::string& filepath, int error_code)
{
	std::ostringstream	err_code_str;
	std::stringstream	file_buff;
	std::ifstream		fs;
//	std::string			filepath = dir_path;
	std::string			status_msg;
	std::string			header;
	std::string			body;
	std::ostringstream	text_buff;
	std::ostringstream	content_length;
	//size_t				header_size, response_size;//, content_length;
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

	text_buff << "HTTP/1.1 ";//"404 OK\r\n";
	text_buff << status_msg + "\r\n";
	text_buff << "Content-type: text/html; charset=utf-8\r\n";

//	file_buff << fs.rdbuf();
//	body += file_buff.str();

	fs.seekg(0, std::ios::end);
//	content_length << fs.tellg();
//	content_length << body.length();
	text_buff << "Content-length: ";
	text_buff << fs.tellg();//content_length.str();
	fs.seekg(0, std::ios::beg);
	text_buff << "\r\n\r\n";
//	header_size = text_buff.tellp();
	text_buff << fs.rdbuf();
	fs.close();
//	response_size = text_buff.tellp();
//	content_length = response_size - header_size;
//	std::cout << body;
//	content_length << body.length();
	//Find content_length
//	file_buff.seekp(0, std::ios::end);
//	content_length << file_buff.tellp();
//	file_buff.seekp(0, std::ios::beg);
//	std::cout << "Error Page Content Length : " << content_length.str() << std::endl;
//	body += file_buff.str();
//	while (std::getline(fs, line))
//		body += line;

	
	//this->_text = header + fs.read(0)(//file_buff.str();//body;
	this->_text = text_buff.str();//file_buff.str();//body;
	return (true);
}
*/
/*
/// Add more as needed or just call __prepare_error directly.
bool	ErrorResponse::_prepare_default_201_error(void) {return (__prepare_error("default_errors/", 201));}
bool	ErrorResponse::_prepare_default_204_error(void) {return (__prepare_error("default_errors/", 204));}
bool	ErrorResponse::_prepare_default_301_error(void) {return (__prepare_error("default_errors/", 301));}
bool	ErrorResponse::_prepare_default_404_error(void) {return (__prepare_error("default_errors/", 404));}
bool	ErrorResponse::_prepare_default_413_error(void) {return (__prepare_error("default_errors/", 413));}
bool	ErrorResponse::_prepare_default_500_error(void) {return (__prepare_error("default_errors/", 500));}
*/

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
	std::cout << "Custom error filepath found : " << ret_path << std::endl;
	std::cout << "access test (== 0 is good) : " << access(ret_path.c_str(), F_OK | R_OK) << std::endl;
	return (access(ret_path.c_str(), F_OK | R_OK));
}

/// REQUIRED METHODS BY SERVER ///////////////
void	ErrorResponse::prepare_response(int error_code)
{
	std::cout << "Preparing ErrorResponse." << std::endl;
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
