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


# define MAX_READ_BUFF 4096

# define SRVHTTP_DEFAULT_NAME		""
# define SRVHTTP_DEFAULT_ROOT		"/"
# define SRVHTTP_DEFAULT_INADDR		"0.0.0.0"
# define SRVHTTP_DEFAULT_PORT		(uint16_t)80
# define SRVHTTP_DEFAULT_TIMEOUT	10000

# define SRVHTTP_MAX_SEND_RETRIES	10;


class ServerHTTP: public AServerDispatchSwitch
{
	private:
		const std::string					_rootdir;
		std::ostringstream					_raw_request;
		Request								rq;
		//Response								rp;

		// Series of functions called by serve_request()
		int		receive_request(int clientfd, Request& request);
		int		send_response(int clientfd, const Response& resp) const;
		int		_serve_internal_error(int clientfd, const Request& req, const ServerConfig& cfg) const;

	public:
		ServerHTTP(const std::string& servname=SRVHTTP_DEFAULT_NAME, const std::string& rootdir=SRVHTTP_DEFAULT_ROOT,
			const std::string& ip=SRVHTTP_DEFAULT_INADDR, uint16_t port=SRVHTTP_DEFAULT_PORT,
			int timeout=SRVHTTP_DEFAULT_TIMEOUT, const ServerConfig* cfg=NULL);

		virtual ~ServerHTTP(void);

		int		start(void);
		int		serve_request(int clientfd);

		t_srv_state	*get_srv_state(void);
		const std::vector<LocationConfig>&		get_srv_locations(void) const;
		const std::vector<LocationConfig>*		get_srv_locations(const std::string& host) const;// Returns pointer instead of ref to indicate if requested host exists.
		const std::string&						get_server_name(void) const;
		const std::string&						get_rootdir(void) const;

		const ServerConfig&	get_config_for_host(const std::string& host) const;
		bool				add_virtual_server(const IServer& other);
};

#endif
