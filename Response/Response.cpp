/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:42:23 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/24 18:09:11 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sys/stat.h>


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

std::map<int, std::string>	status_msgs = status_msgs_init();



Response::Response(void): _error_code(0), _requested_endpoint(false),
	_requested_autoindex(false)
{
}
Response::~Response(void) {}

enum FileType {
	FILE_TYPE_DIRECTORY,
	FILE_TYPE_FILE,
	FILE_TYPE_UNKNOWN
};


static bool fileExists(const std::string &f)
{
//	std::ifstream file(f.c_str());
//	return (file.good());
	return (access(f.c_str(), F_OK) == 0);
}

static FileType pathType(const std::string& path)
{
	struct stat s;

	if (stat(path.c_str(), &s) != 0)
	{
		return FILE_TYPE_UNKNOWN;
	}
	if (S_ISDIR(s.st_mode))
	{
		return FILE_TYPE_DIRECTORY;
	} else if (S_ISREG(s.st_mode))
	{
		return FILE_TYPE_FILE;
	}
	else 
	{
		return FILE_TYPE_UNKNOWN;
	}
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

void _build_redirect_http_header(const std::string& redirectLocation, std::string& header) {

    // Include the Location header within the HTML content
    //header = "HTTP/1.1 301 Moved Permanently\r\n";
    header = "HTTP/1.1 " + status_msgs[301] + "\r\n";
    header += "Content-Type: text/html\r\n";
    header += "Location: " + redirectLocation + "\r\n";
    header += "\r\n";
}

// Assumes the file existe is valide and accessible.
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
	struct stat stat_s;

	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;
//	std::cout << "_location_path : " << _location_path << std::endl;

	if (_requested_endpoint)
	{
		if (loc_cfg.GetIndexFile().empty())
			_internal_path += "/index.html";
		else
			_internal_path += loc_cfg.GetIndexFile();
	}

	std::cout << "GET REQUEST internal path : " << _internal_path << std::endl;

	if (stat(_internal_path.c_str(), &stat_s) < 0
		|| !(stat_s.st_mode & S_IFREG))
	{
		_error_code = 404;
		return (false);
	}
	else if (access(_internal_path.c_str(), F_OK) < 0)
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
	fs.open(_internal_path.c_str());
	if (!fs.is_open())
	{
		/// 500 Internal Error ... Or something else.
		std::cerr << "ERROR 500 Not Found" << std::endl;
		Logger::log(LOG_DEBUG, "ERROR 500 Internal Error");
		// At this point the filepath should be confirmed valid, so not being able to open it is unexpected.
		_error_code = 500;
		return (false);
	}
	std::cout << "File open correctly for get request." << std::endl;
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

static bool	_validate_body_size(const std::string& maxBodySize, size_t content_length)
{
	size_t	max_body_size;
//	std::vector<std::string>	trimed_str;

//	split_string(maxBodySize, ' ', trimed_str);
//	if (trimed_str.size() == 0)
//		return (Logger::log(LOG_ERROR, "max_body_size has no value."), -1);

//	if (!is_all_digits(maxBodySize))
//		return (Logger::log(LOG_ERROR, "max_body_size should contain only digits."), -1);

	char	*end_ptr = NULL;
	max_body_size = std::strtol(maxBodySize.c_str(), &end_ptr, 10);//maxBodySize);
	return (content_length <= max_body_size);
}

bool Response::_process_post_request(const Request &req, const ServerConfig &srv_cfg, const LocationConfig &loc_cfg)
{
	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;
	const std::string &post_data = req.get_body();

	if (!loc_cfg.IsAllowedMethod(req.get_method()))
	{
		_error_code = 405;// Method Not Allowed
		return (false);		
	}
	if (!_validate_body_size(loc_cfg.GetMaxBodySize(), req.get_content_length()))
	{
		_error_code = 413;// Payload too large
		return (false);
	}

	std::cout << "HANDELING POST REQUEST !" << std::endl;

	std::string response_body = "Received post data:\n" + post_data;
	std::string header;
	std::string	filepath = _internal_path;
	_error_code = 0;
	if (req.getMultiformFlag())
	{
		std::cout << "IS MULTIPART !" << std::endl;
		std::vector<DataPart> dataparts;
		req.extract_multipart(dataparts);
		for (size_t i = 0; i < dataparts.size(); i++)
		{
			if (dataparts[i].getFilename() == "")
				continue;

			
			filepath = _internal_path;
			if (string_endswith(_internal_path, "/"))
				filepath += dataparts[i].getFilename();
			else
				filepath += std::string("/") + dataparts[i].getFilename();

			string_replace_space_by__(filepath);
			if (fileExists(filepath))
			{
//				std::cerr << "ERROR : File already exists" << std::endl;
				_error_code = 409;
				break;
			}
//			std::cout << "Creating New Uploaded file : " << filepath << std::endl;

			std::ofstream file(filepath.c_str(), std::ios::binary);
			if (file.fail())
			{
				_error_code = 500;
				break;
			}
			else
			{
				file.write(dataparts[i].getContent().c_str(), dataparts[i].getContent().length());
			}
		}
	}
	// else
	// {
	// 	std::cout << "ooooooooooooooooooo  no multipart" << std::endl;

	// 	// filepath = _internal_path;
	// 	// if (string_endswith(_internal_path, "/"))
	// 	// 	filepath += dataparts[i].getFilename();
	// 	// else
	// 	// 	filepath += std::string("/") + dataparts[i].getFilename();
	// 	if (fileExists(_internal_path))
	// 	{
	// 		_error_code = 409;
	// 	}
	// 	else
	// 	{
	// 		std::ofstream file(_internal_path.c_str(), std::ios::binary);
	// 		if (file.fail())
	// 		{
	// 			_error_code = 500;
	// 		}
	// 		else
	// 		{
	// 			std::cout << "IS NOT MULTIPART !" << std::endl;
	// 			std::cout << "body length : " << req.get_body().length() << std::endl;
	// 			//std::cout << req << std::endl;
	// 			file.write(req.get_body().c_str(), req.get_body().length());
	// 		}
	// 	}
	// }
	_error_code = 201;
	//_build_get_http_header("", header, std::to_string(response_body.length()), "text/plain", false);
	//	std::cout << "response body : " << response_body << std::endl;
	//_text = header + response_body;

	return (false);
}

bool Response::_process_delete_request(const Request &req, const ServerConfig &srv_cfg, const LocationConfig &loc_cfg)
{
	(void)req;
	(void)srv_cfg;
	(void)loc_cfg;

	if (access(_internal_path.c_str(), F_OK) < 0)
	{
		_error_code = 404;
		return (false);
	}
	if (pathType(_internal_path) == FILE_TYPE_FILE)
	{
		if (remove(_internal_path.c_str()) == 0)
			_error_code = 204;
		else
			_error_code = 403;
//		std::cout << "A DELETE WAS DONE !! TIME TO REMOOOOOVE" << std::endl;
	}
	else
		_error_code = 405;
	return (false);
}

bool Response::_isredirect(const LocationConfig &loc_cfg)
{
	std::string location;
	std::string header;
	std::cout << " CHECK IF REDIRECT " << std::endl;
	std::cout << " RETURN PATH " + loc_cfg.GetReturnPath() << std::endl;
	if (!loc_cfg.GetReturnPath().empty())
	{
		_error_code = 301;
		location = loc_cfg.GetReturnPath();
//		if (location[0] != '/')
//			location.insert(location.begin(), '/');
			
//		std::cout << "**********************************" << std::endl;
		_build_redirect_http_header(location, header);
		std::cout << location << std::endl;
		std::cout << header << std::endl;
	    _text = header;
		return true; // Indicate that redirection is taking place
	}
	std::cout << " IS NOT REDIRECT " << std::endl;
	return false; // Indicate that redirection is not occurring
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
	if (loc_path == req_path
		|| (req_path.compare(0, req_path.length(), loc_path) == 0))
	//		&& req_path.find('/') == loc_path.length()))
	{
		_requested_endpoint = true;
		//std::cout << "loc_path == req_path : " << loc_path << " vs " << req_path << " || compare : " << req_path.compare(0, loc_path.length(), loc_path) << std::endl;
		if (_requested_endpoint && (loc_cfg.GetAutoIndex() == "on"))
		{
			std::cout << "IS AUTOINDEX" << std::endl;
			_requested_autoindex = true;
		}
//		else 
//			std::cout << "IS NOT AUTOINDEX" << std::endl;
//		std::cout << "IS REQUESTED ENDPOINT !!" << std::endl;
	}
	//else
	//	std::cout << "loc_path != req_path : " << loc_path << " vs " << req_path << ". compare : " << req_path.compare(0, req_path.length(), loc_path) << std::endl;
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
		std::cerr << "Response :: _process_cgi_request returns -1 after cgi constructor failure " << std::endl;
		return (-1);
	}

//	(void)req;
	(void)srv_cfg;
	//(void)cgi;
	std::cout << "CGI REQUEST WAS PROCESSED" << std::endl;
	
	if (cgi.run() < 0)// Because we give cgi the response._text reference in its constructor, the response is writen right in the response's _text and is ready to get.
	{
		_error_code = cgi.get_error_code();
		std::cerr << "Response :: _process_cgi_request returns -1 after cg.run() failure " << std::endl;
		return (-1);
	}
	return (0);
}

/// END OF CGI RELATED METHODS


static int	_build_autoindex_body(const Request& req, const std::string& dirpath, std::string& body)
{
	DIR				*odir;//opendir(const char *name);
	struct dirent	*ent;
	std::string		ent_str, site_path;
	

	std::cout << "_build_autoindex_body start" << std::endl;
	
	odir = opendir(dirpath.c_str());
	if (!odir)
		return (Logger::log(LOG_WARNING, std::string("Couldn't open and autoindex directory at ") + dirpath), -1);
	body += std::string(\
	"<!DOCTYPE html>\r\n") +\
	"<html>\r\n" +\
	"<head>\r\n" +\
	"	<title>Webserv Index</title>\r\n" +\
	"</head>\r\n" +\
	"<body>\r\n" +\
	"	<h1>Directory Listing</h1>\r\n";

	site_path = req.get_path();
	if (!string_endswith(site_path, "/"))
		site_path += '/';
	while ((ent = readdir(odir)))
	{
		if (ent->d_name[0] == '.')
			continue ;
		ent_str = ent->d_name;
		if (ent->d_type == DT_DIR)
			ent_str += '/';
//		std::cout << "read dirent : " << (site_path + ent_str) << std::endl;
		body += "	<a href=" + (site_path + ent_str) + ">" + ent_str + "</a><br>\r\n";
	}

	
	body += std::string(\
	"</body>\r\n") +\
	"</html>\r\n";
	closedir(odir);
	return (0);
}

int	Response::_prepare_autoindex(const Request& req, const std::string& dirpath)
{
	std::string		header, cur_timestamp, body;
	std::ostringstream	os;

	std::cout << "REQUESTED AND PREPARING AUTOINDEX " << std::endl;

	if (_build_autoindex_body(req, dirpath, body) < 0)
		return (-1);
	gen_timestamp(cur_timestamp);
	header = "HTTP/1.1 200 OK\r\n";
	header += "Date: " + cur_timestamp + "\r\n";

	os << body.length();
	header += "Content-type: text/html\r\n";
	header += "Content-length: " + os.str() + "\r\n";

	header += "\r\n";
	_text = header + body;
	return (0);
}


/// REQUIRED METHODS BY SERVER ///////////////
int Response::prepare_response(const ServerHTTP &srv, const Request &req, const ServerConfig &cfg)
{
	(void)srv;
//	(void)req;
//	(void)cfg;
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
		_error_code = 500;
		return (-1);
	}
	else
	{
		//		const LocationConfig* bestLocation = cfg.GetLocations().at(locationIndex);
		// bestLocation.print();
		//std::cout << "Beast location match :" << std::endl;
		//best_match->print();

		if (!_validate_request(req, cfg, *best_match))
		{
			std::cerr << "Failed to validate request !!" << std::endl;
			return (-1);
		}

		std::cout << status_msgs[301] << std::endl;

		if (_isredirect(*best_match))
		{
			/*
			std::string header;
			std::string response_body="Redirect Error";
			_build_get_http_header("", header, std::to_string(response_body.length()), "text/plain", false);
			_text = header + response_body;
			std::cerr << "Redirect " << std::endl;
			return (0);
			*/

			std::cout << "A REDIRECT WAS DONE !!" << std::endl;
			//std::cout << this->get_response() << std::endl;
			return (0);
			/*
			std::cout << "A REDIRECT WAS DONE !!" << std::endl;
			return(-1);
			*/
		}

		std::cout << "cwd : " << ServerConfig::cwd << std::endl;
		_parse_location_path(cfg, *best_match);
		_parse_internal_path(req, *best_match);
		
//		std::cout << "Parsed location path : " << _location_path << std::endl;
		//_location_path = ServerConfig::cwd + (*best_match).GetPath();
		//		_location_path = ServerConfig::cwd + (*best_match).GetRoot();

//		std::cout << std::endl
//				  << "Best match path : " << (*best_match).GetPath() << std::endl;
//		std::cout << "Requested path : " << req.get_path() << std::endl;
		std::cout << "Location path : " << _location_path << std::endl;
		

		if (_check_if_cgi_exec(req, *best_match))
			return (_process_cgi_request(req, cfg, *best_match));


		if (req.get_method() == "GET")
		{
			if (_requested_autoindex)
			{
				if (_prepare_autoindex(req, _internal_path) < 0)
				{
					_error_code = 500;
					return (-1);
				}
			}
			else if (!_process_get_request(req, cfg, *best_match))
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
		{
			_error_code = 405;
			std::cerr << "METHOD NOT RECOGNIZED !" << std::endl;
		}
	}

	// DUMMY RESPONSE. DELETE ME
	//_prepare_dummy_response(_text);
	return (0);
}

const std::string &Response::get_response(void) const { return (this->_text); }
/// END OF REQUIRED METHODS BY SERVER /////////////

bool Response::empty(void) const { return (this->_text.empty()); }

int Response::get_error_code(void) const { return (this->_error_code); }
