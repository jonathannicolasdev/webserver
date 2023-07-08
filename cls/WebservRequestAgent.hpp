/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservRequestAgent.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 19:07:50 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/07 22:37:11 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_REQUEST_AGENT_HPP
# define WEBSERV_REQUEST_AGENT_HPP

# include <cstdlib>

struct s_ws_request_header
{
	size_t  raw_rqst_size;
	char    raw_request[];// Open ended array containing client request content.
};

class WebservRequestAgent
{
	public:
		
};

#endif