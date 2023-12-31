/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIAgent.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/16 19:30:10 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/25 19:25:22 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIAgent.hpp"

int	close_pipes(int *pp1, int *pp2, bool err)
{
	if (pp1)
	{
		if (pp1[0] > 2)
		{
			close(pp1[0]);
			pp1[0] = -1;
		}
		else
		{
			close(pp1[1]);
			pp1[1] = -1;
		}
	}
	if (pp2)
	{
		if (pp2[0] > 2)
		{
			close(pp2[0]);
			pp2[0] = -1;
		}
		else
		{
			close(pp2[1]);
			pp2[1] = -1;
		}
	}
	if (err)
		return (-1);
	return (0);
}

int	close_pipe(int *pp)
{
	if (pp && *pp > 2)
	{
		close(*pp);
		*pp = -1;
	}
	return (0);
}


void	_build_cgi_http_header(const std::string& filepath, std::string& header,
	const std::string& content_length, const std::string& content_type, bool is_attachment)
{
	std::string			cur_timestamp, last_modified_time;
	
	gen_timestamp(cur_timestamp);
	header = "HTTP/1.1 200 OK\r\n";
	header += "Date: " + cur_timestamp + "\r\n";
	
	if (!content_type.empty())
		header += "Content-type: " + content_type + "\r\n";
	header += "Content-length: " + content_length + "\r\n";
	
	// Makes GET file downloadable rather then displayable.
	if (is_attachment)
	{
		header += "Cache-Control: private\r\n";
		header += "Content-Disposition: attachment;";

		header += " filename=\"";
		size_t	last_slash = filepath.find_last_of('/');
		if (last_slash == filepath.npos)
			header += filepath;
		else
			header += filepath.substr(filepath.find_last_of('/') + 1);
		header += "\"\r\n";
	}
	header += "\r\n";
}

const std::string&	pwd(std::string& ret_string)
{
	char	buff[1028];

	getcwd(buff, 1028);
	ret_string = buff;
	return (ret_string);
}

void	extract_content_from_pipe(int read_pipe, std::string& ret)
{
	char	buff[4096];
	ssize_t	len;

	while ((len = read(read_pipe, buff, 4095)) > 0)
	{
		buff[len] = '\0';
		ret += buff;
	}
}


bool	_validate_path(const std::string& intern_path, int& error_code,
	std::string& dir_path, std::string& script_name, std::string& path_info)
{
	size_t					sct_pos = intern_path.find(".py");
	const std::string		script_path = intern_path.substr(0, sct_pos + 3);
	size_t					dir_pos = intern_path.rfind('/', sct_pos);

	dir_path = intern_path.substr(0, dir_pos + 1);
	script_name = script_path.substr(dir_path.length(), script_path.length() - dir_path.length());
	path_info = intern_path.substr(script_path.length());

	// if (DEBUG)
	// {
	// 	std::cout << "CGI :: dir_path : " << dir_path << std::endl;
	// 	std::cout << "CGI :: script_path : " << script_path << std::endl;
	// 	std::cout << "CGI :: script_name : " << script_name << std::endl;
	// 	std::cout << "CGI :: path_info : " << path_info << std::endl;
	// }

	if (access(script_path.c_str(), F_OK) < 0)
	{
		std::cerr << "CGI :: access F_OK : FAILED" << std::endl;
		error_code = 404;
		return (false);
	}
	if (access(script_path.c_str(), X_OK) < 0)
	{
		std::cerr << "CGI :: access X_OK : FAILED" << std::endl;
		error_code = 500;
		return (false);
	}
	return (true);
}

CGIAgent::CGIAgent(const Request& req, const LocationConfig& loc_srv, 
	const std::string& abs_internal_path, std::string& response):
	_req(req),
	_srv_cfg(loc_srv.GetServerConfig()), _loc_cfg(loc_srv), 
	_error_code(0), _text(response)
{
	std::vector<std::string>::iterator	it;

	env_strs.push_back(std::string("PATH_INFO=") + abs_internal_path);
	env_strs.push_back(std::string("SCRIPT_FILENAME=") + abs_internal_path);
	env_strs.push_back(std::string("REQUEST_METHOD=") + req.get_method());
	env_strs.push_back(std::string("QUERY_STRING=") + req.get_query());
	env_strs.push_back(std::string("CONTENT_TYPE=") + req["Content-Type"]);
	env_strs.push_back(std::string("CONTENT_LENGTH=") + req.get_content_length_str());
	
	if (_validate_path(abs_internal_path, _error_code, _dir_path, _script_name, _path_info_short))
	{
		// if (DEBUG)
		// {
		// 	std::cout << "CGI Agent constructor" << std::endl;
		// 	std::cout << "CGI abs_internal_path : " << abs_internal_path << std::endl;
		// 	std::cout << "CGI dir_path : " << _dir_path << std::endl;
		// 	std::cout << "CGI _script_name : " << _script_name << std::endl;
		// }
		
		_script_internal_path = abs_internal_path;

		_argv.push_back("/usr/bin/python3");
		_argv.push_back(_script_name.c_str());
		
		for (it=env_strs.begin(); it != env_strs.end(); ++it)
			_env.push_back(it->c_str());
	}
	if (_argv.size() == 0)
		_argv.push_back("/usr/bin/python3");
	_argv.push_back(NULL);
	_env.push_back(NULL);
}

CGIAgent::~CGIAgent(void) {}

int
CGIAgent::_child_process(int *cgi_send_pipe, int *cgi_read_pipe)
{
	std::string			pwd_str;

	if (chdir(_dir_path.c_str()) < 0)
	{
		Logger::log(LOG_ERROR, "Child process failed to change directory to script location.");
		exit(EXIT_FAILURE);
	}

	close_pipe(&cgi_send_pipe[1]);
	close_pipe(&cgi_read_pipe[0]);
	
	if (_req.get_content_length())
	{
		dup2(cgi_send_pipe[0], 0);
	}
	else
	{
		close_pipe(&cgi_send_pipe[0]);
	}
	dup2(cgi_read_pipe[1], 1);

	if (execve(_argv[0], (char * const *)&_argv[0], (char * const *)&_env[0]) < 0)
		perror("execve failed ");
	std::cerr << "Child pipe duped" << std::endl;
	Logger::log(LOG_ERROR, "execve() call for cgi execition failed.");
	close_pipes(cgi_send_pipe, cgi_read_pipe, true);
	exit(EXIT_FAILURE);
}

/// PARENT PROCESS MONITORING CGI
int	CGIAgent::_parent_process(int pid, int *cgi_send_pipe, int *cgi_read_pipe, const std::string& content_type)
{
	int	status;
	int	waitpid_ret;
	std::ostringstream	content_length;
	std::string			body;
	std::string			pwd_str;

	close_pipe(&cgi_send_pipe[0]);
	close_pipe(&cgi_read_pipe[1]);

	if (_req.get_content_length())
	{
		size_t	to_send = _req.get_content_length();
		size_t	sent = 0, total_sent = 0;
		while ((sent = write(cgi_send_pipe[1], _req.get_body().c_str() + total_sent, to_send)) > 0)
		{
			to_send -= sent;
			total_sent += sent;
		}
	}
	close_pipe(&cgi_send_pipe[1]);

	time_t	start_time = std::time(NULL), cur_time = start_time;
	while ((waitpid_ret = waitpid(pid, &status, WNOHANG)) == 0)
	{
		usleep(10000);
		cur_time = std::time(NULL);
		if ((cur_time - start_time) >= MAX_CGI_TIMEWAIT)
		{
			Logger::log(LOG_WARNING, "Gateway took too long to respond.");
			_error_code = 504;
			kill(pid, SIGKILL);
			return (close_pipes(cgi_send_pipe, cgi_read_pipe, true));
		}
	}
	if (waitpid_ret == -1)
	{
		Logger::log(LOG_WARNING, "CGI waitpid FAILED");
		_error_code = 500;
		kill(pid, SIGKILL);
		return (close_pipes(cgi_send_pipe, cgi_read_pipe, true));
	}
	else if (WEXITSTATUS(status) != EXIT_SUCCESS)
	{
		Logger::log(LOG_WARNING, "CGI returned with error code");
		_error_code = 500;
		return (close_pipes(cgi_send_pipe, cgi_read_pipe, true));
	}
	extract_content_from_pipe(cgi_read_pipe[0], body);
	close_pipe(&cgi_read_pipe[0]);
	content_length << body.length();

	_build_cgi_http_header("", _text, content_length.str(), content_type, false);
	_text += body;
	close_pipes(cgi_send_pipe, cgi_read_pipe, true);
	return (0);
}

int     CGIAgent::run(void)
{
	int	pid;
	int cgi_send_pipe[2] = {0, 0};
	int cgi_read_pipe[2] = {0, 0};


	if (_error_code)
	{
		Logger::log(LOG_ERROR, "CGI cannot be run if an error_code was triggered.");
		return (-1);
	}
	if (pipe(cgi_send_pipe) < 0 || pipe(cgi_read_pipe) < 0)
	{
		_error_code = 500;
		return (close_pipes(cgi_send_pipe, cgi_read_pipe, true));
	}
	if ((pid = fork()) < 0)
	{
		_error_code = 500;
		return (close_pipes(cgi_send_pipe, cgi_read_pipe, true));
	}
	else if (pid == 0)
	{
		/// CHILD PROCESS EXECUTING CGI
		_child_process(cgi_send_pipe, cgi_read_pipe);
	}
	else
	{
		/// PARENT PROCESS EXECUTING CGI
		if (_parent_process(pid, cgi_send_pipe, cgi_read_pipe, "text/html") < 0)
			return (close_pipes(cgi_send_pipe, cgi_read_pipe, true));
	}
	return (close_pipes(cgi_send_pipe, cgi_read_pipe, false));
}

int	CGIAgent::get_error_code(void) const {return (_error_code);}
const std::string&	CGIAgent::get_response(void) const {return (_text);}
