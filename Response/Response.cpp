/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:42:23 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/15 17:05:27 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void) {}
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
	return (true);
}
/*
bool Response::_process_post_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg)
{
	(void)srv_cfg;  // Unused parameter
	(void)loc_cfg;  // Unused parameter
	// Check if the target file exists
	if (fileExists(_target_file))
	{
		_error_code = 204; // No Content
		return true;
	}
	// Attempt to open the target file for writing
	std::ofstream file(_target_file.c_str(), std::ios::binary);
	if (file.fail())
	{
		_error_code = 404; // Not Found
		return false;
	}
	// Check if the request contains multipart form data
	if (req.getMultiformFlag())
	{
		// Remove boundary and extract the body
		std::string body = req.getBody();
		body = removeBoundary(body, req.getBoundary());
		// Write the processed body to the file
		file.write(body.c_str(), body.length());
	}
	else
	{
		// Write the request body directly to the file
		file.write(req.getBody().c_str(), req.getBody().length());
	}
	// Prepare a response indicating successful upload
	std::string response_body = "File uploaded successfully.";
	std::string header;
	_build_get_http_header("", header, std::to_string(response_body.length()), "text/plain", false);
	_text = header + response_body;
	// Set the HTTP response code to 201 (Created)
	_error_code = 201;
	return true;
}
*/

bool Response::_process_post_request(const Request &req, const ServerConfig &srv_cfg, const LocationConfig &loc_cfg)
{
	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;
	const std::string &post_data = req.get_body();
	std::string response_body = "Received post data:\n" + post_data;
	std::string header;

	if (fileExists(_internal_path))
	{
		_error_code = 204;
		return (0);
	}
	std::ofstream file(_internal_path.c_str(), std::ios::binary);
	if (file.fail())
	{
		_error_code = 404;
		return (1);
	}

	if (req.getMultiformFlag())
	{
		std::string body = req.get_body();
		std::vector<DataPart> dataparts = req.extract_multipart();
		for (size_t i = 0; i < dataparts.size(); i++)
		{
			if (dataparts[i].getName()=="file")
			{
				std::ofstream file(dataparts[i].getFilename().c_str(), std::ios::binary);
				if (file.fail())
				{
					_error_code = 404;
					// decide if you have to continue or stop
				}
				file.write(dataparts[i].getContent().c_str(), dataparts[i].getContent().length());
			}
			else { //case of name=description

			}

		}

		file.write(body.c_str(), body.length());
	}
	else
	{
		file.write(req.get_body().c_str(), req.get_body().length());
	}

	_build_get_http_header("", header, std::to_string(response_body.length()), "text/plain", false);
	std::cout << "response body : " << response_body << std::endl;
	_text = header + response_body;
	_error_code = 200;

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
	const std::string &req_path = req.get_path();
	const std::string &loc_path = loc_cfg.GetPath();
	std::string spec_requested = req_path.substr(loc_path.length());
	//	std::vector<std::string>	req_path_split;
	//	std::vector<std::string>	loc_path_split;
	std::vector<std::string> spec_path_split;

	//	std::string					full_internal_path = _location_path;
	//(void)req;
	//	spec_requested = req_path.substr(loc_path.length());

	_internal_path = _location_path;
	//	split_string(req_path, '/', req_path_split);
	//	split_string(loc_path, '/', loc_path_split);
	split_string(spec_requested, '/', spec_path_split);

	//	join_strings(req_path_split, '/', full_internal_path);
	//	join_strings(loc_path_split, '/', full_internal_path);
	//	join_strings(spec_path_split, '/', full_internal_path);
	join_strings(spec_path_split, '/', _internal_path);

	//	std::cout << "spec_requested : " << spec_requested << std::endl;
	//	_internal_path = full_internal_path;//_location_path + spec_requested;
	if (loc_path == req_path) // && loc_cfg.GetIndexFile())
	{
		_requested_endpoint = true;
		std::cout << "IS REQUESTED ENDPOINT !!" << std::endl;
		//		_internal_path += loc_cfg.GetIndexFile();
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

/// REQUIRED METHODS BY SERVER ///////////////
int Response::prepare_response(const ServerHTTP &srv, const Request &req, const ServerConfig &cfg)
{
	(void)srv;
	(void)req;
	(void)cfg;
	const LocationConfig *best_match;

	std::cout << " ******************* PATH:" << req.get_path() << "\n";
	std::cout << " ******************* SERVERCONFIG :";
	std::cout << " ******************* cwd : " << ServerConfig::cwd << std::endl;
	cfg.print();
	std::cout << "Preparing Response." << std::endl;
	// int locationIndex = cfg.getBestLocationMatch(req.get_path());
	std::cout << "Request path : " << req.get_path() << std::endl;
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

const std::string &Response::get_response(void) const { return (this->_text); }
/// END OF REQUIRED METHODS BY SERVER /////////////

bool Response::empty(void) const { return (this->_text.empty()); }

int Response::get_error_code(void) const { return (this->_error_code); }
