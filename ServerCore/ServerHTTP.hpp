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
//# include <map>
# include <vector>
# include <set>
# include <ctime>
//# include <cstring>
# include <ostream>
# include <string>
# include <unistd.h>
# include <fcntl.h>
# include <netdb.h>

//# include "IServer.hpp"
# include "AServerDispatchSwitch.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ErrorResponse.hpp"
# include "Logger.hpp"

# define SRVHTTP_DEFAULT_NAME		""
# define SRVHTTP_DEFAULT_ROOT		"/"
# define SRVHTTP_DEFAULT_INADDR		"0.0.0.0"
# define SRVHTTP_DEFAULT_PORT		(uint16_t)80
# define SRVHTTP_DEFAULT_TIMEOUT	10000

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
		
		//ServerConfig						_cfg;
//		std::map<std::string, std::string>	_locations;
		std::ostringstream					_raw_request;
		Request								rq;
		//Response								rp;

		// Series of functions called by serve_request()
		int		receive_request(int clientfd, Request& request);
//		int		parse_request(Request& request) const;// Can take Request ref from the rq attribute or from and locally declared Request instance.
		//int	prepare_response(Request& request, Response& response) const;
		//int		send_response(int clientfd);//, Response& response) const;
		int		send_response(int clientfd, const Response& resp) const;
		int		_serve_internal_error(int clientfd, const Request& req, const ServerConfig& cfg) const;

	public:
		ServerHTTP(const std::string& servname=SRVHTTP_DEFAULT_NAME, const std::string& rootdir=SRVHTTP_DEFAULT_ROOT,
			const std::string& ip=SRVHTTP_DEFAULT_INADDR, uint16_t port=SRVHTTP_DEFAULT_PORT,
			int timeout=SRVHTTP_DEFAULT_TIMEOUT, const ServerConfig* cfg=NULL);

		virtual ~ServerHTTP(void);

		int		start(void);
//		int		parse_request(int clientfd);// raw_request should have been read already.
		int		serve_request(int clientfd);

		t_srv_state	*get_srv_state(void);
//		const std::map<std::string, std::string>&	get_srv_locations(void) const;
		const std::vector<LocationConfig>&		get_srv_locations(void) const;
		const std::vector<LocationConfig>*		get_srv_locations(const std::string& host) const;// Returns pointer instead of ref to indicate if requested host exists.
		const std::string&						get_server_name(void) const;
		const std::string&						get_rootdir(void) const;

		const ServerConfig&	get_config_for_host(const std::string& host) const;
		bool				add_virtual_server(const IServer& other);

		//bool	add_virtual_server(const ServerHTTP& other);
//		bool	add_virtual_server(const IServer& other);

};

#endif
