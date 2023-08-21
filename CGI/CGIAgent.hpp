/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIAgent.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 20:47:22 by marvin            #+#    #+#             */
/*   Updated: 2023/08/11 20:47:22 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIAGENT_HPP
# define CGIAGENT_HPP

# include <unistd.h>
# include <sys/wait.h>
# include <stdlib.h>
# include <signal.h>
# include <iostream>
# include <string>
# include <vector>
# include <ctime>
# include <sstream>

# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "Request.hpp"

# define MAX_CGI_TIMEWAIT 2// in sec

struct CGIConfig
{
	std::string		path_info;
	std::string		host;
	std::string		content_type;
};


class CGIAgent
{
	private:
		const Request&			_req;
		const ServerConfig&		_srv_cfg;
		const LocationConfig&	_loc_cfg;


		std::string					_script_internal_path;
//		char**						_script_args;
//		char**						_script_env;
		std::vector<const char *>	_argv;
		std::vector<const char *>	_env;

		std::vector<std::string>	env_strs;
		std::vector<char*>			env_vect;
		int							_error_code;

		// std::string					_dir_path;
		// std::string					_script_name;
		// std::string					_header;
		// std::string&				_text;

		std::string					_dir_path;
		std::string					_script_name;
		std::string					_path_info;
		std::string					_path_info_short;
		std::string					_header;
		std::string&				_text;


		int	_child_process(int *cgi_send_pipe, int *cgi_read_pipe);
		int	_parent_process(int *cgi_send_pipe, int *cgi_read_pipe);
		int	_parent_process(int pid, int *cgi_send_pipe, int *cgi_read_pipe, const std::string& content_type);

	public:
//		CGIAgent(void);
	//	CGIAgent(const Request& req, const LocationConfig& loc_srv, const std::string& rel_internal_path, const std::string& abs_internal_path, std::string& response);
		CGIAgent(const Request& req, const LocationConfig& loc_srv, const std::string& abs_internal_path, std::string& response);
		//CGIAgent(const Request& req);
		~CGIAgent(void);

		int	run(void);
//		int	run(const std::string& rel_internal_path);

		int	get_error_code() const;
		const std::string&	get_response() const;
};

#endif