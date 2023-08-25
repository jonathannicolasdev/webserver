/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:40:57 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/25 16:21:17 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP


# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
# include <string>
# include <fstream>
# include <sstream>

# include "Request.hpp"
# include "CGIAgent.hpp"
# include "ServerConfig.hpp"
# include "webserv_utils.hpp"

extern std::map<int, std::string> status_msgs;

class ServerHTTP;

class Response
{
	private:
		friend class	CGIAgent;

	protected:
		std::string		_text;
		int				_error_code;
		std::string		_server_path;// absolute path of server_root
		std::string		_location_path;// absolute path of location match 
		std::string		_internal_path;// absolute path inside server of requested path converted to server path with best matching LocationConfig. (eg.: req_path = "/admin/toto.html", location "/admin/" => "/var/www/admin", internal_path = "/var/www/admin/toto.html") 
		bool			_requested_endpoint;// The path requested is the exact match of the location config
		bool			_requested_autoindex;// The path requested endpoint and autoindex is on

		bool	_process_get_request(const LocationConfig& loc_cfg);
		bool	_process_post_request(const Request& req, const LocationConfig& loc_cfg);
		bool 	_process_delete_request(const std::string& filepath, const LocationConfig& loc_cfg);
		bool	_isredirect(const LocationConfig &loc_cfg);
		bool	_validate_request(const Request& req, const LocationConfig& loc_cfg);
		std::string&	_parse_internal_path(const Request& req, const LocationConfig& loc_cfg);
		std::string &	_parse_location_path(const ServerConfig& srv_cfg, const LocationConfig &loc_cfg);
		bool			_check_if_cgi_exec(const Request& req);
		int				_process_cgi_request(const Request& req, const LocationConfig& loc_cfg);
		int				_prepare_autoindex(const Request& req, const std::string& dirpath);

	public:
		Response(void);    
		virtual ~Response(void);

		int prepare_response(const Request& req, const ServerConfig& cfg);
		const std::string&	get_response(void) const;
		bool	empty(void) const;
		int		get_error_code(void) const;		
};

#endif
