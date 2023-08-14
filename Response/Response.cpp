/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:42:23 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/13 23:48:05 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void) {}
Response::~Response(void) {}

void	_prepare_dummy_response(std::string& text_response)
{
	std::string			header;
	std::string			body;
	std::ostringstream	content_length;
	std::string			response;

	header = "HTTP/1.1 200 OK\r\n";
	header += "Content-type: text/html\r\n";
	
	body +=	"<!DOCTYPE html>\r\n";
	body +=	"<html>\r\n";
	body +=	"	<head>\r\n";
	body +=	"		<title>Webserv Index</title>\r\n";
	body +=	"	</head>\r\n";
	body +=	"	<body>\r\n";
	body +=	"		<h1>Hello World page1</h1>\r\n";
	body +=	"		<img src=\"asdf.jpg\" width=100 height=100>\r\n";
	body +=	"		<a href=\"page2.html\">My beautiful link</a>\r\n";
	body +=	"	</body>\r\n";
	body +=	"</html>\r\n";

	header += "Content-length: ";
	content_length << body.length();
	header += content_length.str() ;
	header += "\r\n\r\n";
	text_response = header + body;
}


void	_build_get_http_header(const std::string& filepath, std::string& header,
	const std::string& content_length, const std::string& content_type)
{
	std::string			cur_timestamp, last_modified_time;
	
	gen_timestamp(cur_timestamp);
	get_file_last_modified_time(filepath, last_modified_time);
	header = "HTTP/1.1 200 OK\r\n";
	header += "Date: " + cur_timestamp + "\r\n";
	
	header += "Content-type: " + content_type + "\r\n";
	header += "Content-length: " + content_length + "\r\n";
	header += "Last-Modified: " + last_modified_time + "\r\n";
	
	/// Had this to send the file as a downloadable attachment rather then a display item.
//	header += std::string("Content-Disposition: attachment; filename=\"example.jpg\"") + "\r\n";	
	header += "\r\n";
	/// ..., maybe
}

/// GET method is confirmed valid at this stage.
bool	Response::_process_get_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg)
{
	std::ifstream		fs;
	std::ostringstream	os, content_length;
	std::string			header, body, content_type;

	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;
	std::cout << "_location_path : " << _location_path << std::endl;
	
	if (_requested_endpoint)
	{
		if (loc_cfg.GetIndexFile()[0] != '/')
			_internal_path += '/';
		_internal_path += loc_cfg.GetIndexFile();
	}

	std::cout << "GET REQUEST internal path : " << _internal_path << std::endl;
	
	if (access(_internal_path.c_str(), F_OK) < 0)
	{
		/// 403 File not found
		_error_code = 404;
		//...
		return (false);
	}
	else if (access(_internal_path.c_str(), R_OK) < 0)
	{
		/// 403 Forbiden
		_error_code = 403;
		//...
		return (false);
	}
	fs.open(_internal_path);
	if (!fs.is_open())
	{
		/// 500 Internal Error ... Or something else.
		// At this point the filepath should be confirmed valid, so not being able to open it is unexpected.
		_error_code = 500;
		return (false);
	}
	os << fs.rdbuf();
	fs.close();
	body = os.str();
	content_length << body.length();

	/// DEBUG ONLY. ACTUALLY FIND THE CORRECT CONTENT TYPE.
	//content_type = "image/jpeg";
	content_type = "text/html";

	_build_get_http_header(_internal_path, header, content_length.str(), content_type);
	this->_text = header + body;
	std::cout << std::endl << "RESPONSE HEADER : " << std::endl;
	std::cout << header << std::endl;
//	std::cout << "** ------------------------ [BUILT RESPONSE] -------------------------- **" << std::endl;
//	std::cout << this->_text << std::endl;
//	std::cout << "** ---------------------- [BUILT RESPONSE END] -------------------------- **" << std::endl;
	return (true);
}

bool	Response::_process_post_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg)
{
	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;
	return (true);
}

bool	Response::_process_delete_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg)
{
	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;
	return (true);
}

bool	Response::_validate_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg)
{
	(void)srv_cfg;
	if (!loc_cfg.IsAllowedMethod(req.get_method()))
	{
		std::cout << "request method : " << req.get_method() << ", Is NOT allowed" << std::endl;
		_error_code = 405;// Method NOT ALLOWED;
		return (false);
	}
	std::cout << "request method : " << req.get_method() << ", IS allowed" << std::endl;
	//...
	return (true);
}

/// If this is called, we know the location path matches the requested path at least in part, but the path may not be valid.
void	Response::_get_internal_path(const Request& req, const LocationConfig& loc_cfg)
{
	const std::string&	req_path = req.get_path();
	const std::string&	loc_path = loc_cfg.GetPath();
	std::string			spec_requested;
	//(void)req;
	spec_requested = req_path.substr(loc_path.length());
	std::cout << "spec_requested : " << spec_requested << std::endl;
	_internal_path = _location_path + spec_requested;
	if (loc_path == req_path)// && loc_cfg.GetIndexFile())
	{
		_requested_endpoint = true;
		std::cout << "IS REQUESTED ENDPOINT !!" << std::endl;
//		_internal_path += loc_cfg.GetIndexFile();
	}
	std::cout << "Full internal path : " << _internal_path << std::endl;
}

/// REQUIRED METHODS BY SERVER ///////////////
int Response::prepare_response(const ServerHTTP& srv, const Request& req, const ServerConfig& cfg)
{
	(void)srv;
	(void)req;
	(void)cfg;
	const LocationConfig*	best_match;

	std::cout << " ******************* PATH:" << req.get_path() <<"\n";
	std::cout << " ******************* SERVERCONFIG :"  ;
	std::cout << " ******************* cwd : " << ServerConfig::cwd << std::endl;
	cfg.print();
	std::cout << "Preparing Response." << std::endl;
	//int locationIndex = cfg.getBestLocationMatch(req.get_path());
	best_match = cfg.getBestLocationMatch(req.get_path());
//	if (locationIndex==-1)
	if (best_match == NULL)
	{
		std::cout << "no location found";
	}
	else {
//		const LocationConfig* bestLocation = cfg.GetLocations().at(locationIndex);
		//bestLocation.print();
		best_match->print();

		/// IAN EXTRA
		if (!_validate_request(req, cfg, *best_match))
		{
			std::cerr << "Failed to validate request !!" << std::endl;
			return (-1);
		}
		std::cout << "cwd : " << ServerConfig::cwd << std::endl;
		//_location_path = ServerConfig::cwd + (*best_match).GetPath();
		_location_path = ServerConfig::cwd + (*best_match).GetRoot();
		
		std::cout << std::endl << "Best match path : " << (*best_match).GetPath() << std::endl;
		std::cout << "Requested path : " << req.get_path() << std::endl;
		std::cout << "Location path : " << _location_path << std::endl;
		_get_internal_path(req, *best_match);

		if (req.get_method() == "GET")
			_process_get_request(req, cfg, *best_match);
		else if (req.get_method() == "POST")
			_process_post_request(req, cfg, *best_match);
		else if (req.get_method() == "DELETE")
			_process_delete_request(req, cfg, *best_match);
	}

	// DUMMY RESPONSE. DELETE ME
	//_prepare_dummy_response(_text);
	return (0);
}

const std::string&	Response::get_response(void) const {return (this->_text);}
/// END OF REQUIRED METHODS BY SERVER /////////////

bool
Response::empty(void) const {return (this->_text.empty());}

int		Response::get_error_code(void) const {return (this->_error_code);}
