/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIAgent.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/16 19:30:10 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/21 17:19:15 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIAgent.hpp"

int	close_pipe(int *pp, bool err)
{
	if (pp[0])
		close(pp[0]);
	else
		close(pp[1]);
	if (err)
		return (-1);
	return (0);
}


//void	_build_cgi_http_header(const std::string& filepath, std::string& header,
void	_build_cgi_http_header(const std::string& filepath, std::string& header,
	const std::string& content_length, const std::string& content_type, bool is_attachment)
{
	std::string			cur_timestamp, last_modified_time;
	
	gen_timestamp(cur_timestamp);
//	get_file_last_modified_time(filepath, last_modified_time);
	header = "HTTP/1.1 200 OK\r\n";
	header += "Date: " + cur_timestamp + "\r\n";
	
	if (!content_type.empty())
		header += "Content-type: " + content_type + "\r\n";
	header += "Content-length: " + content_length + "\r\n";
//	header += "Last-Modified: " + last_modified_time + "\r\n";
	
	/// Had this to send the file as a downloadable attachment rather then a display item.
	//header += std::string("Content-Disposition: attachment; filename=\"example.jpg\"") + "\r\n";
	if (is_attachment)// Makes GET file downloadable rather then displayable.
	{
		header += "Cache-Control: private\r\n";
		header += "Content-Disposition: attachment;";

		header += " filename=\"";// + "example.jpg" + "\"";
		size_t	last_slash = filepath.find_last_of('/');
		if (last_slash == filepath.npos)
			header += filepath;
		else
			header += filepath.substr(filepath.find_last_of('/') + 1);
		header += "\"\r\n";
	}
	header += "\r\n";
	/// ..., maybe
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
	std::cout << "CGI :: dir_path : " << dir_path << std::endl;
	std::cout << "CGI :: script_path : " << script_path << std::endl;
	std::cout << "CGI :: script_name : " << script_name << std::endl;
	std::cout << "CGI :: path_info : " << path_info << std::endl;
		
//	size_t		pos = intern_path.find_last_of('/');
//	dir_path = abs_internal_path.substr(0, pos + 1);
//	script_name = abs_internal_path.substr(pos + 1);


	std::cout << "CGI :: access F_OK : " << std::endl;
	if (access(script_path.c_str(), F_OK) < 0)
	{
		std::cout << "CGI :: access F_OK : FAILED" << std::endl;
		error_code = 404;
		return (false);
	}
	std::cout << "CGI :: access F_OK : OK" << std::endl;
	std::cout << "CGI :: access X_OK : " << std::endl;
	if (access(script_path.c_str(), X_OK) < 0)
	{
		std::cout << "CGI :: access X_OK : FAILED" << std::endl;
		error_code = 500;
		return (false);
	}
	std::cout << "CGI :: access X_OK : OK" << std::endl;
	return (true);
}

CGIAgent::CGIAgent(const Request& req, const LocationConfig& loc_srv, 
	const std::string& abs_internal_path, std::string& response):
	//_req(req),
	_srv_cfg(loc_srv.GetServerConfig()), _loc_cfg(loc_srv), 
	_error_code(0), _text(response)
{
	std::vector<std::string>::iterator	it;
	std::vector<std::string>	env_strs;

	env_strs.push_back(std::string("PATH_INFO=") + abs_internal_path);
	env_strs.push_back(std::string("SCRIPT_FILENAME=") + abs_internal_path);
	env_strs.push_back(std::string("REQUEST_METHOD=") + req.get_method());
	env_strs.push_back(std::string("QUERY_STRING=") + req.get_query());
	env_strs.push_back(std::string("CONTENT_TYPE=") + req["Content-Type"]);
	env_strs.push_back(std::string("CONTENT_LENGTH=") + req.get_content_length_str());
	
	if (_validate_path(abs_internal_path, _error_code, _dir_path, _script_name, _path_info_short))
	{
//		(void)_req;
		(void)_loc_cfg;
		std::cout << "CGI Agent constructor" << std::endl;
		std::cout << "CGI abs_internal_path : " << abs_internal_path << std::endl;
		std::cout << "CGI dir_path : " << _dir_path << std::endl;
		std::cout << "CGI _script_name : " << _script_name << std::endl;
		
		_script_internal_path = abs_internal_path;

		_argv.push_back("/usr/bin/python3");
		_argv.push_back(_script_name.c_str());
		
		std::cout << "" << std::endl;

		for (it=env_strs.begin(); it != env_strs.end(); ++it)
			_env.push_back(it->c_str());
		
		
	//	_script_args = &_argv[0];
		/// DEBUG
	//	std::cout << _req << std::endl;
	}
	std::cout << "CGI constructor after validate path " << std::endl;
	if (_argv.size() == 0)
		_argv.push_back("/usr/bin/python3");
	std::cout << "CGI constructor after _argv[0] check" << std::endl;
	_argv.push_back(NULL);
	_env.push_back(NULL);
	std::cout << "CGI constructor exit " << std::endl;
}

CGIAgent::~CGIAgent(void)
{
}

int     CGIAgent::run(void)
{
	int pid;
	int pp[2] = {0, 0};
	int	status;
	int	waitpid_ret;
	std::ostringstream	content_length;
	std::string			body;
	std::string			pwd_str;

	if (_error_code)
	{
		Logger::log(LOG_ERROR, "CGI cannot be run if an error_code was triggered.");
		return (-1);
	}
	if (pipe(pp) < 0)
	{
		_error_code = 500;
		return (close_pipe(pp, true));
	}
	std::cout << "CGI FORKING PROCESS" << std::endl;
	if ((pid = fork()) < 0)
	{
		_error_code = 500;
		return (close_pipe(pp, true));
	}
	else if (pid == 0)
	{
		/// CHILD PROCESS EXECUTING CGI

		std::cout << "HERE IS CHILD PROCESS" << std::endl;
		if (chdir(_dir_path.c_str()) < 0)
		{
			Logger::log(LOG_ERROR, "Child process failed to change directory to script location.");
			std::cerr << "script location : " << _dir_path << std::endl;
			exit(EXIT_FAILURE);
		}
		std::cout << "Child pwd after chdir : " << pwd(pwd_str) << std::endl;
		close(pp[0]);
		std::cout << "Child pipe read closed." << std::endl;
		pp[0] = 0;
		dup2(pp[1], 1);
		std::cerr << "_argv[0] : " << _argv[0] << std::endl;
//		std::cerr << "_env[0] : " << _env[0] << std::endl;
		std::cerr << "Child pipe duped" << std::endl;

		//std::cout << "CGI CALLING EXECVE" << std::endl;
		if (execve(_argv[0], (char * const *)&_argv[0], (char * const *)&_env[0]) < 0)
		{
			std::cerr << "CGI EXECVE FAILED !! " << std::endl;
			std::cerr << "_script_internal_path was : " << _script_internal_path << std::endl;
			perror("execve failed ");
//			return (close_pipe(pp, true));
		}
		std::cerr << "Child pipe duped" << std::endl;
		Logger::log(LOG_ERROR, "execve() call for cgi execition failed.");
		close_pipe(pp, true);
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "HERE IS PARENT PROCESS" << std::endl;
		/// PARENT PROCESS MONITORING CGI
		close(pp[1]);


		time_t	start_time = std::time(NULL), cur_time = start_time;

		while ((waitpid_ret = waitpid(pid, &status, WNOHANG)) == 0)
		{
			usleep(10000);
			cur_time = std::time(NULL);
			if ((cur_time - start_time) >= MAX_CGI_TIMEWAIT)
			{
				//std::cerr << "CGI TOOK TOO LONG. EXIT WITH INTERNAL ERROR." << std::endl;
				Logger::log(LOG_WARNING, "Gateway took too long to respond.");
				_error_code = 504;
				kill(pid, SIGKILL);
				return (close_pipe(pp, true));
			}
		}
		if (waitpid_ret == -1)
		{
			perror("Waitpid FAILED");
			_error_code = 500;
			kill(pid, SIGKILL);
			return (close_pipe(pp, true));
		}
		else if (waitpid_ret == pid)
		{
			std::cout << "waitpid worked as expected. Requested CGI and parend process returned successfully." << std::endl;
		}
		extract_content_from_pipe(pp[0], body);
		//os << pp[0];
		//std::cout << "Returned from CGI pipe : " << os.str() << std::endl;
		std::cout << "Returned from CGI pipe : " << std::endl << body << std::endl;
		close(pp[0]);
//		body = os.str();
		content_length << body.length();

		/// BUILDING CGI HEADER
		_build_cgi_http_header("", _text, content_length.str(), "text/html", false);
		_text += body;
	}
	return (close_pipe(pp, false));
}

int	CGIAgent::get_error_code(void) const {return (_error_code);}
const std::string&	CGIAgent::get_response(void) const {return (_text);}
