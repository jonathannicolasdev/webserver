/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 17:27:52 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/22 22:36:33 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HTTP_HPP
# define SERVER_HTTP_HPP

# include <cerrno>
# include <map>
# include <set>
# include <ctime>
//# include <cstring>
# include <string>
# include <unistd.h>
# include <fcntl.h>

//# include "IServer.hpp"
# include "AServerDispatchSwitch.hpp"
# include "Request.hpp"
# include "Logger.hpp"

/*
# define MAX_PENDING_CONN 128

enum e_conn_status
{
	CLT_ACCEPTED,
	CLT_PROCESSING,
	CLT_CLOSED
};

typedef struct s_client_connection_state
{
	int						clt_fd;
	struct sockaddr_in		clt_addr;
	enum e_conn_status		conn_status;
	std::time_t					init_conn_time;
	std::time_t					last_act_time;
	std::time_t					time_on_site;// now - init_conn_time. Used to timeout connections potentially.
	std::time_t					time_since_active;// now - last_act_time. Used to timeout connections potentially.
//	double					act_per_sec;// Potential implementation to prevent DDoS attacks.
}	t_clt_conn;

// Potentialy add some enum describing cause of timout.
struct s_timeout_info
{
	std::time_t	start_time;
	std::time_t	duration;
	std::time_t	end_time;
};
*/
/*
class	ServerHTTP: public AServerReactive
{
	private:
		const std::string			_rootdir;
		const std::string			_server_name;

		//  Uses hash map to store client's connection state info
		// with connection fd (returned by accept()) for key
		// and tracking client activity on site with the connection 
		// state struct defined above. Once the client
//		std::map<int, t_clt_conn>	_active_connections;

		//  Potentially implement ip bans and ip timeouts.
		std::set<in_addr_t>							_addr_blacklist;
		std::map<in_addr_t, struct s_timeout_info>	_addr_timeouts;

		//   Returns false if client is in blacklist or is timed out. If the timeout
		// check finds that the timeout period is over, the address is removed from
		// _addr_timeouts and returns true so connection can be established.
		bool	_validate_client_connection(struct sockaddr_in& addr);

	public:
		ServerHTTP(const std::string& srv_name, const std::string& addr, uint16_t port=PORT_HTTP, const std::string &rootdir="./");
		virtual ~ServerHTTP();
	
		int					bind_server(void);
		int					start(void);
		void				stop(void);
		void				disconnect(int clt_fd);
		t_srv_state			*get_srv_state(void);
		const std::string&	get_server_name(void) const;
		const std::string&	get_rootdir(void) const;
		std::map<std::string, std::string>&	get_srv_locations(void);
};
std::ostream&	operator<<(std::ostream& ostream, ServerHTTP& srv);
*/

// DEBUG SIMPLE VERSION
class ServerHTTP: public AServerDispatchSwitch
{
	private:
		const std::string					_rootdir;
		const std::string					_server_name;
		std::map<std::string, std::string>	_locations;

	public:
		ServerHTTP(const std::string& rootdir, const std::string& servname,
			const std::string& ip, uint16_t port);
		virtual ~ServerHTTP(void);

		int	start(void);

		int	parse_request(int clientfd, Request& request) const;
		//int	serve_request(int clientfd) const;
//		int	serve_response(int clientfd) const;

//		virtual int		bind_server(void);

		t_srv_state	*get_srv_state(void);
		const std::map<std::string, std::string>&	get_srv_locations(void) const;
		const std::string&							get_server_name(void) const;
		const std::string&							get_rootdir(void) const;
};

#endif
