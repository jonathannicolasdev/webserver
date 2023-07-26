/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 17:19:50 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/22 20:51:02 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <unistd.h>
# include <iostream>
# include <ctime>
# include <limits.h>

//# include "Request.hpp"
# include "ServerHTTP.hpp"
# include "AServerCluster.hpp"
#include "ConfigBuilder.hpp"
#include "ServerConfig.hpp"
#include "Logger.hpp"
//# include "ClusterWeb.hpp"

# define UNUSED(o) (void)(o)

/*
typedef struct s_webserv
{
//	ClusterWeb	web_cluster;
	bool		is_init;
	bool		flag_global_termination;
	ServerHTTP  *srv;
}	t_webs;


t_webs  *get_webserv_main_struct(void);
*/
// Signal handlers
//void	webs_sigint_handler(int signum);

// Webserv utils
void		gen_timestamp(std::string& ret);
std::string get_working_path(void);

#endif
