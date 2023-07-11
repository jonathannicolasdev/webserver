/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservRequestAgent.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 19:07:50 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/10 20:36:41 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_REQUEST_AGENT_HPP
# define WEBSERV_REQUEST_AGENT_HPP

# include <cstdlib>
# include <sstream>
# include <string>
# include <map>
# include <cunistd>
# include <iostream>
//# include <queue>

# include "Logger.hpp"
# include "IServer.hpp"

# define MAX_REG_SRVS 128

typedef struct s_ws_request_processing_header
{
	int		sockfd;
	int		connfd;
	size_t  content_size;
//	char    content[];// Open ended array containing client request content.
}	t_ws_req;

class WebservRequestAgent
{
	private:
		int						_pipe[2];
		t_ws_req				_processing_req;
		std::stringstream		_req_content;
		
		// Each server has to register to the agent before being able to push
		// requests through the pipe. The agent keeps a copy of each server's 
		// config to properly handle their client's request.
		std::map<std::string, std::string>	_server_location_cfgs[MAX_REG_SRVS];
	
		//size_t	get_content_length(std::stringstream& content);

	public:
		WebservRequestAgent();//const IServer& srv, std::map<std::string, std::string> locations);
		~WebservRequestAgent();
		int	register_server_to_agent(IServer& srv);
		int	push_request(int sockfd, int connfd, const std::string req_content);
//		const int	get_input_pipe(void) const;
		int	get_response_pipe(void) const;
};

#endif