/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IServer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 17:02:09 by marvin            #+#    #+#             */
/*   Updated: 2023/07/05 17:02:09 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISERVER_HPP
# define ISERVER_HPP

# include <string>
# include <iostream>
# include <csignal>
# include <map>
# include <stdint.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
//# include <netinet/ip.h>

enum e_server_status_codes
{
	SRV_UNBOUND,
	SRV_IDLE,
	SRV_LISTENING,
	SRV_BROKEN
};

enum e_common_port
{
	PORT_HTTP = 80,
	PORT_HTTPS = 443,
	PORT_FTP = 20,
	PORT_SSH = 22	
};

typedef struct s_server_state
{
	bool					is_running;
	uint16_t				port;
	std::string				address;// as string. ex : "127.0.0.1"
	std::string				status;// set with srv_status_to_str(enum status) call
	
	// allows for specific server types to add a pointer to a struct of relevant data to this type.
	// Can be a pointer to a struct defined in the concrete server implemenetation file and declared
	// in the class itself.
	void					*extra;
}	t_srv_state;

class   IServer
{
	protected:
//		AServerCuster*				_owner;/// Value should be set to owner cluster if server is
										// attached to a cluster else defaults to NULL.
		int							_sockfd;
		uint16_t					_port;
		struct sockaddr_in			_server_addr;
		bool						_close_request;
		bool						_is_running;
		enum e_server_status_codes	_status;

		// A pointer to this struct is returned by get_srv_state() call. The specific
		// implementation of get_srv_state should be implemented in the concrete server classes.
		t_srv_state             _srv_state_view;

		IServer(uint16_t _port, bool _close_rqst, bool _is_running, enum e_server_status_codes _status);
		
	public:
		virtual int			bind_server(void) = 0;
		virtual int			start(bool self_managed) = 0;
		virtual void		stop(void) = 0;
		virtual t_srv_state	*get_srv_state(void) = 0;
};

std::string &srv_status_to_str(enum e_server_status_codes status, std::string &ret_str);
/*
IServer::IServer(uint16_t _port, bool _close_rqst, bool _is_running, int _status):
	_port(_port), _close_request(_close_rqst), _is_running(_is_running), _status(_status)
{
	std::cout << "IServer interface protected constructor called" << std::endl;
}
*/

#endif