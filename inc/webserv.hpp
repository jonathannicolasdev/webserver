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

# include <iostream>
# include <vector> 

//# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>

# include "Server.hpp"
# include "AServerCluster.hpp"
# include "ClusterWeb.hpp"

# define UNUSED(o) (void)(o)

typedef struct s_webserv
{
	ClusterWeb	web_cluster;
}	t_webs;

#endif
