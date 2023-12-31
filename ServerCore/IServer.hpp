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
# include <cstring>
# include <iostream>
# include <map>
# include <stdint.h>
# include <sys/socket.h>

# include <netinet/in.h>
# include <arpa/inet.h>

# include "ServerConfig.hpp"

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
	std::string				server_name;
	bool					is_running;
	uint16_t				port;
	std::string				address;// as string. ex : "127.0.0.1"
	enum e_server_status_codes	status_code;// current status enum for the server
	std::string				status;// set with srv_status_to_str(enum status) call
	int						open_connections;// nb of clients currently connected to it.
	// allows for specific server types to add a pointer to a struct of relevant data to this type.
	// Can be a pointer to a struct defined in the concrete server implemenetation file and declared
	// in the class itself.
	void					*extra;
}	t_srv_state;


// This class is seperate from IServer to have all subclasses be able to 
// call eachother's get_sockfd() methods, but limit accessibilty of the socket 
// only to derived classes of __BaseSocketOwner. The purpose is to have AServerCluster
// be able to call an IServer.get_sockfd() method to manage its servers with poll,
// but prevent any other classes not inheriting __BaseSocketOwner to get access 
// to a server's socket directly and be able to close it prematurly for exemple.
class	__BaseSocketOwner
{
	private:
		friend class AServerCluster;
	protected:
		int		_sockfd;
		int		get_sockfd(void) const;
};


class   IServer:	public __BaseSocketOwner
{
	private:
		friend class	ServerFactory;

	protected:
		std::string					_server_name;
		std::vector<ServerConfig>	_cfgs;
		uint16_t					_port;
		struct sockaddr_in			_server_addr;
		bool						_close_request;
		bool						_is_running;
		bool						_is_dispatch_switch;
		bool						_is_reactive;
		bool						_is_multi_domain;// If multiple server configs are run from the same port.
		enum e_server_status_codes	_status;

		std::map<std::string, ServerConfig>	configs;// takes hostname as key and returns its config.

		// A pointer to this struct is returned by get_srv_state() call. The specific
		// implementation of get_srv_state should be implemented in the concrete server classes.
		t_srv_state             _srv_state_view;

		IServer(uint16_t _port, bool _close_rqst, bool _is_running, bool _is_reactive, bool _is_dispatch_switch, enum e_server_status_codes _status);

		bool		is_dispatch_switch(void) const;
		bool		is_reactive(void) const;


	public:
		virtual ~IServer(void);
		virtual int			bind_server(void) = 0;
		virtual int			start(void) = 0;
		virtual void		stop(void) = 0;
		virtual t_srv_state	*get_srv_state(void) = 0;
		virtual bool		is_serving(int client_fd) const = 0;// if concrete server does not track client connection state (is stateless), implement with return (false);.

		virtual const std::vector<ServerConfig>&	get_config(void) const = 0;
		virtual const std::string&	get_server_name(void) const = 0;
		virtual uint16_t			get_port(void) const = 0;
		virtual in_addr_t			get_addr(void) const = 0;
		virtual bool				is_running(void) const = 0;
	
		// Merges both configs in this server. Adds a reachable domain name on this server with its
		// own configuration and contant. Both need to be open on same network interface.
		virtual bool		add_virtual_server(const IServer& other) = 0;
		
		virtual bool		operator==(const IServer& other) const = 0;
};

std::string &srv_status_to_str(enum e_server_status_codes status, std::string &ret_str);

#endif