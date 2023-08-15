/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:40:57 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/14 18:36:49 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP


# include <unistd.h>
# include <fcntl.h>
# include <fstream>
# include <sstream>

# include "Request.hpp"
//# include "ServerHTTP.hpp"
# include "ServerConfig.hpp"
# include "webserv_utils.hpp"

class ServerHTTP;

class Response
{
	protected:
		std::string		_text;
		int				_error_code;
		std::string		_server_path;// absolute path of server_root
		std::string		_location_path;// absolute path of location match 
		std::string		_internal_path;// absolute path inside server of requested path converted to server path with best matching LocationConfig. (eg.: req_path = "/admin/toto.html", location "/admin/" => "/var/www/admin", internal_path = "/var/www/admin/toto.html") 
		bool			_requested_endpoint;// The path requested is the exact match of the location config

		bool	_process_get_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg);
		bool	_process_post_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg);
		bool	_process_delete_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg);

		bool	_validate_request(const Request& req, const ServerConfig& srv_cfg, const LocationConfig& loc_cfg);
		std::string&	_parse_internal_path(const Request& req, const LocationConfig& loc_cfg);
		std::string&	_parse_location_path(const ServerConfig& scfg, const LocationConfig& lcfg);
		
	public:
		Response(void);    
		virtual ~Response(void);

		//int prepare_response(const Request& req);
		int prepare_response(const ServerHTTP& srv, const Request& req, const ServerConfig& cfg);
		const std::string&	get_response(void) const;

		bool	empty(void) const;
		int		get_error_code(void) const;
};

#endif
