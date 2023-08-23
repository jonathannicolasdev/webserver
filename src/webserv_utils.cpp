/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_utils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 16:43:14 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/10 17:03:23 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv_utils.hpp"

/*
// Probably required globally accessible application componants for signal handling
// and clean exits for closing all sockets properly.
t_webs  *get_webserv_main_struct(void)
{
	static t_webs	webs;
	
	if (!webs.is_init)
	{
		// init webserv main as needed
		webs.is_init = true;
	}
	return (&webs);
}
*/

bool	string_endswith(const std::string& fullstr, const std::string& ending)
{
	if (ending.length() > fullstr.length())
		return (false);
	return (fullstr.compare(fullstr.length() - ending.length(), ending.length(), ending) == 0);
}

void	join_strings(const std::vector<std::string>& sv, char delim, std::string& ret_str)
{
	std::vector<std::string>::const_iterator	it;
	
	if (sv.size() == 0)
		return ;

//	std::cout << "join_strings entered with ret_str : " << ret_str << std::endl;

	for (it = sv.begin(); it != sv.end(); ++it)
	{
		if ((*it).empty())
			continue ;
		ret_str += delim;
		ret_str += *it;
	}
//	std::cout << "join_strings exits with ret_str : " << ret_str << std::endl;
}

void	split_string(const std::string& s, char delim, std::vector<std::string>& ret_vect)
{
	std::vector<std::string>::iterator	it;
	size_t	p1 = 0, p2 = 0;

	if (s.length() == 0)
		return ;

	while ((p2 = s.find(delim, p1)) != s.npos)
	{
		if (p2 > p1)
			ret_vect.push_back(s.substr(p1, p2 - p1));
		p2 = s.find_first_not_of(delim, p2);
		p1 = p2;
	}
	if (p1 < (s.length() - 1))
		ret_vect.push_back(s.substr(p1, p2 - p1));
}

void	split_string(const std::string& s, const std::string& delim, std::vector<std::string>& ret_vect)
{
	std::vector<std::string>::iterator	it;
	size_t	p1 = 0, p2 = 0;

	if (s.length() == 0)
		return ;

	while ((p2 = s.find(delim, p1)) != s.npos)
	{
		if (p2 > p1)
			ret_vect.push_back(s.substr(p1, p2 - p1));
		p2 += delim.length();
//		p2 = s.find_first_not_of(delim, p2);
		p1 = p2;
	}
	if (p1 < (s.length() - delim.length()))
		ret_vect.push_back(s.substr(p1, p2 - p1));
}


bool	is_all_digits(const std::string& s)
{
	return (s.find_first_not_of("0123456789") == s.npos);
}

bool	get_file_last_modified_time(const std::string& filepath, std::string& ret)
{
	char			time_buff[128];
	struct stat		result;
	struct tm		*timeinfo;

	if (stat(filepath.c_str(), &result) < 0)
		return (false);
	timeinfo = std::gmtime(&result.st_mtime);
	strftime(time_buff, 128, "%c GMT", timeinfo);
	ret = time_buff;
	return (true);
}

void    gen_timestamp(std::string& ret)
{
	char					time_buff[128];
	struct tm				*timeinfo;
//	std::stringstream       timestamp;
	std::time_t				t;
	
	std::time(&t);
	timeinfo = std::gmtime(&t);
//	timeinfo = localtime(&t);
//	strftime(time_buff, 80, "%H:%M:%S %d/%m/%Y", timeinfo);
//	strftime(time_buff, 80, "%a, %d %m %Y %T GMT", timeinfo);
	strftime(time_buff, 128, "%c GMT", timeinfo);
	ret = time_buff;
//        timestamp << std::put_time(std::localtime(&t), "%Y%m%d_%H%M%S");
//	timestamp << std::put_time(std::localtime(&t), "%H:%M:%S %d/%m/%Y");
//	ret = timestamp.str();
}

std::string get_working_path(void)
{
   char temp[PATH_MAX];

   return (getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string(""));
}
