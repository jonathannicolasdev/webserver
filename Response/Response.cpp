/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:42:23 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/21 18:54:19 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void): _error_code(0), _requested_endpoint(false)
{
}

Response::~Response(void) {}

static bool fileExists(const std::string &f)
{
	std::ifstream file(f.c_str());
	return (file.good());
}

void _prepare_dummy_response(std::string &text_response)
{
	std::string header;
	std::string body;
	std::ostringstream content_length;
	std::string response;

	header = "HTTP/1.1 200 OK\r\n";
	header += "Content-type: text/html\r\n";

	body += "<!DOCTYPE html>\r\n";
	body += "<html>\r\n";
	body += "	<head>\r\n";
	body += "		<title>Webserv Index</title>\r\n";
	body += "	</head>\r\n";
	body += "	<body>\r\n";
	body += "		<h1>Hello World page1</h1>\r\n";
	body += "		<img src=\"asdf.jpg\" width=100 height=100>\r\n";
	body += "		<a href=\"page2.html\">My beautiful link</a>\r\n";
	body += "	</body>\r\n";
	body += "</html>\r\n";

	header += "Content-length: ";
	content_length << body.length();
	header += content_length.str();
	header += "\r\n\r\n";
	text_response = header + body;
}

const std::string _discover_content_type(const std::string &path)
{
	if (string_endswith(path, ".html"))
		return ("text/html");
	else if (string_endswith(path, ".js"))
		return ("text/javascript");
	else if (string_endswith(path, ".css"))
		return ("text/css");
	else if (string_endswith(path, ".jpg") || string_endswith(path, ".jpeg"))
		return ("image/jpeg");
	else if (string_endswith(path, ".png"))
		return ("image/png");
	else if (string_endswith(path, ".pdf"))
		return ("application/pdf");
	//...
	return ("");
}

void _build_get_http_header(const std::string &filepath, std::string &header,
							const std::string &content_length, const std::string &content_type, bool is_attachment)
{
	std::string cur_timestamp, last_modified_time;

	gen_timestamp(cur_timestamp);
	get_file_last_modified_time(filepath, last_modified_time);
	header = "HTTP/1.1 200 OK\r\n";
	header += "Date: " + cur_timestamp + "\r\n";

	if (!content_type.empty())
		header += "Content-type: " + content_type + "\r\n";
	header += "Content-length: " + content_length + "\r\n";
	header += "Last-Modified: " + last_modified_time + "\r\n";

	/// Had this to send the file as a downloadable attachment rather then a display item.
	// header += std::string("Content-Disposition: attachment; filename=\"example.jpg\"") + "\r\n";
	if (is_attachment) // Makes GET file downloadable rather then displayable.
	{
		header += "Cache-Control: private\r\n";
		header += "Content-Disposition: attachment;";

		header += " filename=\""; // + "example.jpg" + "\"";
		size_t last_slash = filepath.find_last_of('/');
		if (last_slash == filepath.npos)
			header += filepath;
		else
			header += filepath.substr(filepath.find_last_of('/') + 1);
		header += "\"\r\n";
	}
	header += "\r\n";
	/// ..., maybe
}

/// GET method is confirmed valid at this stage.
bool Response::_process_get_request(const Request &req, const ServerConfig &srv_cfg, const LocationConfig &loc_cfg)
{
	std::ifstream fs;
	std::ostringstream os, content_length;
	std::string header, body, content_type;

	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;
	std::cout << "_location_path : " << _location_path << std::endl;

	if (_requested_endpoint)
	{
		if (loc_cfg.GetIndexFile().empty())
			_internal_path += "/index.html";
		else
			_internal_path += loc_cfg.GetIndexFile();
	}

	std::cout << "GET REQUEST internal path : " << _internal_path << std::endl;

	if (access(_internal_path.c_str(), F_OK) < 0)
	{
		/// 404 File not found
		std::cerr << "ERROR 404 Not Found" << std::endl;
		Logger::log(LOG_DEBUG, "ERROR 404 Not Found");
		_error_code = 404;
		//...
		return (false);
	}
	else if (access(_internal_path.c_str(), R_OK) < 0)
	{
		/// 403 Forbidden
		std::cerr << "ERROR 403 Not Found" << std::endl;
		Logger::log(LOG_DEBUG, "ERROR 403 Forbidden");
		_error_code = 403;
		//...
		return (false);
	}
	fs.open(_internal_path);
	if (!fs.is_open())
	{
		/// 500 Internal Error ... Or something else.
		std::cerr << "ERROR 500 Not Found" << std::endl;
		Logger::log(LOG_DEBUG, "ERROR 500 Internal Error");
		// At this point the filepath should be confirmed valid, so not being able to open it is unexpected.
		_error_code = 500;
		return (false);
	}
	os << fs.rdbuf();
	fs.close();
	body = os.str();
	content_length << body.length();

	/// DEBUG ONLY. ACTUALLY FIND THE CORRECT CONTENT TYPE.
	// content_type = "image/jpeg";
	// content_type = req["Content"];//"text/html";

	content_type = _discover_content_type(_internal_path);

	_build_get_http_header(_internal_path, header, content_length.str(), content_type, false); // true);
	this->_text = header + body;
	std::cout << std::endl
			  << "RESPONSE HEADER : " << std::endl;
	std::cout << header << std::endl;
//	std::cout << "** ------------------------ [BUILT RESPONSE] -------------------------- **" << std::endl;
//	std::cout << this->_text << std::endl;
//	std::cout << "** ---------------------- [BUILT RESPONSE END] -------------------------- **" << std::endl;
	std::cout << "GET REQUEST HANDLED" << std::endl;
	return (true);
}

bool Response::_process_post_request(const Request &req, const ServerConfig &srv_cfg, const LocationConfig &loc_cfg)
{
	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;
	const std::string &post_data = req.get_body();

	std::cout << "HANDELING POST REQUEST !" << std::endl;

	std::string response_body = "Received post data:\n" + post_data;
	std::string header;
	_error_code = 0;
	if (req.getMultiformFlag())
	{
		std::cout << "IS MULTIPART !" << std::endl;
		std::vector<DataPart> dataparts = req.extract_multipart();
		for (size_t i = 0; i < dataparts.size(); i++)
		{
			if (dataparts[i].getFilename() == "")
				continue;
			if (fileExists(dataparts[i].getFilename().c_str()))
			{
				_error_code = 204;
				break;
			}
			std::ofstream file(dataparts[i].getFilename().c_str(), std::ios::binary);
			if (file.fail())
			{
				_error_code = 404;
				break;
			}
			else
				file.write(dataparts[i].getContent().c_str(), dataparts[i].getContent().length());
		}
	}
	else
	{
		std::cout << "ooooooooooooooooooo  no multipart" << std::endl;

		if (fileExists(_internal_path))
		{
			_error_code = 204;
		}
		else
		{
			std::ofstream file(_internal_path.c_str(), std::ios::binary);
			if (file.fail())
			{
				_error_code = 404;
			}
			else
			{
				std::cout << "IS NOT MULTIPART !" << std::endl;
				std::cout << "body length : " << req.get_body().length() << std::endl;
				std::cout << req << std::endl;
				file.write(req.get_body().c_str(), req.get_body().length());
			}
		}
	}

	_build_get_http_header("", header, std::to_string(response_body.length()), "text/plain", false);
	//	std::cout << "response body : " << response_body << std::endl;
	_text = header + response_body;


	return (true);
}

bool Response::_process_delete_request(const Request &req, const ServerConfig &srv_cfg, const LocationConfig &loc_cfg)
{
	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;
	return (true);
}

bool Response::_validate_request(const Request &req, const ServerConfig &srv_cfg, const LocationConfig &loc_cfg)
{
	(void)srv_cfg;
	if (!loc_cfg.IsAllowedMethod(req.get_method()))
	{
		std::cout << "request method : " << req.get_method() << ", Is NOT allowed" << std::endl;
		_error_code = 405; // Method NOT ALLOWED;
		return (false);
	}
	std::cout << "request method : " << req.get_method() << ", IS allowed" << std::endl;
	//...
	return (true);
}

/// If this is called, we know the location path matches the requested path at least in part, but the path may not be valid.
std::string &Response::_parse_internal_path(const Request &req, const LocationConfig &loc_cfg)
{
	const std::string&	req_path = req.get_path();
	const std::string&	loc_path = loc_cfg.GetPath();
	std::string			spec_requested = req_path.substr(loc_path.length());
	std::vector<std::string>	spec_path_split;

	_internal_path = _location_path;
	split_string(spec_requested, '/', spec_path_split);
	join_strings(spec_path_split, '/', _internal_path);
	if (loc_path == req_path)
	{
		_requested_endpoint = true;
		std::cout << "IS REQUESTED ENDPOINT !!" << std::endl;
	}
	std::cout << "Full internal path : " << _internal_path << std::endl;
	return (_internal_path);
}

std::string &Response::_parse_location_path(const ServerConfig &scfg, const LocationConfig &lcfg)
{
	_location_path = ServerConfig::cwd;
	join_strings(scfg.GetSplitRoot(), '/', _location_path);
	join_strings(lcfg.GetSplitRoot(), '/', _location_path);
	return (_location_path);
}


/// CGI RELATED METHODS
bool	Response::_check_if_cgi_exec(const Request& req, const LocationConfig& cfg)
{
//	std::vector<std::string>::const_iterator	it = cfg.GetCgiPaths().begin();
//	std::string		path_info = req.get_path();

	(void)req;
	(void)cfg;

	if (string_endswith(req.get_path(), ".py"))
		return (true);
//	size_t	pos = _internal_path.find(".py");
//	if (pos == _internal_path.npos)
//		return (false);
	
//	path_info = _internal_path.substr(pos + 3);
	//std::cout << "_check_if_cgi_exec :: path info : " << path_info << std::endl;
	
//	for (; it != cfg.GetCgiPaths().end(); ++it)
//	{		
//	}
	return (false);
}

int	Response::_process_cgi_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg)
{
	std::string		rel_internal_path;

	if (loc_cfg.GetCgiPaths().size() != 0)
		rel_internal_path = loc_cfg.GetCgiPaths()[0];

	CGIAgent	cgi(req, loc_cfg, _internal_path, _text);
	if (cgi.get_error_code())
	{
		_error_code = cgi.get_error_code();
		std::cerr << "Response :: _process_cgi_request returns -1 after cgi failure " << std::endl;
		return (-1);
	}

//	(void)req;
	(void)srv_cfg;
	//(void)cgi;
	std::cout << "CGI REQUEST WAS PROCESSED" << std::endl;
	
	cgi.run();// Because we give cgi the response._text reference in its constructor, the response is writen right in the response's _text and is ready to get.
	return (0);
}

/// END OF CGI RELATED METHODS



/// REQUIRED METHODS BY SERVER ///////////////
int Response::prepare_response(const ServerHTTP &srv, const Request &req, const ServerConfig &cfg)
{
	(void)srv;
	(void)req;
	(void)cfg;
	const LocationConfig *best_match;

	_error_code = 0;
	// std::cout << " ******************* PATH:" << req.get_path() <<"\n";
	// std::cout << " ******************* SERVERCONFIG :";
	// std::cout << " ******************* cwd : " << ServerConfig::cwd << std::endl;
	//cfg.print();
	std::cout << "Preparing Response." << std::endl;
	// int locationIndex = cfg.getBestLocationMatch(req.get_path());
	//	std::cout << "Request path : " << req.get_path() << std::endl;
	best_match = cfg.getBestLocationMatch(req.get_path());
	//	if (locationIndex==-1)
	if (best_match == NULL)
	{
		std::cout << "no location found";
	}
	else
	{
		//		const LocationConfig* bestLocation = cfg.GetLocations().at(locationIndex);
		// bestLocation.print();
		std::cout << "Beast location match :" << std::endl;
		best_match->print();

		/// IAN EXTRA
		if (!_validate_request(req, cfg, *best_match))
		{
			std::cerr << "Failed to validate request !!" << std::endl;
			return (-1);
		}
		std::cout << "cwd : " << ServerConfig::cwd << std::endl;
		_parse_location_path(cfg, *best_match);
		std::cout << "Parsed location path : " << _location_path << std::endl;
		//_location_path = ServerConfig::cwd + (*best_match).GetPath();
		//		_location_path = ServerConfig::cwd + (*best_match).GetRoot();

		std::cout << std::endl
				  << "Best match path : " << (*best_match).GetPath() << std::endl;
		std::cout << "Requested path : " << req.get_path() << std::endl;
		std::cout << "Location path : " << _location_path << std::endl;
		
		_parse_internal_path(req, *best_match);



		if (_check_if_cgi_exec(req, *best_match))
			return (_process_cgi_request(req, cfg, *best_match));




		if (req.get_method() == "GET")
		{
			if (!_process_get_request(req, cfg, *best_match))
			{
				std::cout << "WOWOW GOT GET METHOD BUT process_get_request FAILED !" << std::endl;
				return (-1);
			}
		}
		else if (req.get_method() == "POST")
		{
			if (!_process_post_request(req, cfg, *best_match))
				return (-1);
		}
		else if (req.get_method() == "DELETE")
		{
			if (!_process_delete_request(req, cfg, *best_match))
				return (-1);
		//	_process_delete_request(req, cfg, *best_match);
		}
		else
			std::cerr << "METHOD NOT RECOGNIZED !" << std::endl;
	}

	// DUMMY RESPONSE. DELETE ME
	//_prepare_dummy_response(_text);
	return (0);
}

const std::string &Response::get_response(void) const { return (this->_text); }
/// END OF REQUIRED METHODS BY SERVER /////////////

bool Response::empty(void) const { return (this->_text.empty()); }

int Response::get_error_code(void) const { return (this->_error_code); }
