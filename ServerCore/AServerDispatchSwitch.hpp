/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServerDispatchSwitch.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 20:44:06 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/10 20:44:06 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef A_SERVER_DISPATCH_SWITCH_HPP
# define A_SERVER_DISPATCH_SWITCH_HPP

# include <cerrno>
# include <cstring>
# include <unistd.h>
# include <sys/time.h>
# include <algorithm>

# include "AServerReactive.hpp"
# include "Logger.hpp"


# define MAX_PENDING_CONN 1024

enum e_conn_status
{
	CLT_ACEPTED,
	CLT_PROCESSING,
	CLT_CLOSED
};

typedef struct s_client_connection_state
{
//	int						sock_fd;
	int						clt_fd;
	struct sockaddr_in		clt_addr;
	enum e_conn_status		conn_status;
	std::time_t					init_conn_time;
	std::time_t					last_act_time;
	std::time_t					time_on_site;// now - init_conn_time. Used to timeout connections potentially.
	std::time_t					time_inactive;// now - last_act_time. Used to timeout connections potentially.
}	t_clt_conn;


// This abstract class provides child servers their networking methods.
// After calling bind_server() that creates the network socket and binds it
// to the port given as argument, calling start() will make it listen to new 
// connections. Once the first client connects its socket descriptor is put in
// the t_clt_conn client connection struct where the state of the clients connection
// will be tracked until they disconnect or the server kicks them out for innactivity.
// Server connections are kept alive if instanciated with 
class	AServerDispatchSwitch: public AServerReactive
{
	protected:

		int		_validate_ready_start(void);
		void	_init_new_client_connection(int clientfd);


		//  Uses hash map to store client's connection state info
		// with connection fd (returned by accept()) for key
		// and tracking client activity on site with the connection 
		// state struct defined above.
		std::map<int, t_clt_conn>	_active_connections;

		// If _keep_alive is true, the server will keep the client connections open 
		// and store them in the _active_connections map, therefore making the server stateful.
		// If false, closes the client connections after each interaction.
		// The struct created in the map will keep track of timestamps indicating
		// init client connection time, last activity timestamp and time since last activity
		// which will be compared against _conn_timout duraction to flush inacive connections.
		bool	_keep_alive;
		int		_conn_timeout;

		int				_pollfd;// will be kqueue fd on MacOS;
		std::time_t		_srv_start_time;
		std::time_t		_last_maintenance_time;
		int				_currently_serving;
		bool			_client_disconnect_signaled;

		// The conn_timeout variable is in ms
		AServerDispatchSwitch(uint16_t _port,
			enum e_server_status_codes _status, bool conn_persistance, int conn_timeout=10000);

		virtual int		disconnect_oldest(int *disconn_clients, int max_disconn);
		

	public:
			
		virtual	~AServerDispatchSwitch();
		virtual int		bind_server(void);
		virtual int		start(void);
		virtual void	stop(void);
		virtual int		disconnect(int clt_fd, bool force);
		virtual void	disconnect_all(bool force);
		virtual void	disconnect_current_client(void);
		virtual void	switch_connection_persistance(void);// Switches the _keep_alive bool on/off. 
		virtual int		connect(int *disconn_clients, int max_disconn, int *ret_clientfd);
		virtual bool	is_running(void) const;
		virtual bool	is_serving(int client_fd) const;
		
		// Still abstract methods to implement in specialized concrete classes.
		virtual int		serve_request(int clientfd) = 0;

		// checks its active_connections and removes clients having exceded their timeout
		// puts the flushed clients fds in the disconn_clients int array limited 
		// by max_disconn. Returns the nb of clients put in disconn_clients.
		// If no pointer is given for disconn_clients, max_disconn is ignored and 
		// server maintenance is performed without returning the filled array of 
		// disconnected clients fds.
		virtual int			do_maintenance(int *disconn_clients, int max_disconn);

		virtual const std::vector<ServerConfig>&	get_config(void) const;
		virtual uint16_t	get_port(void) const;
		virtual in_addr_t	get_addr(void) const;
		virtual int			get_timeout(void) const;
		const t_clt_conn*	get_client_state(int client_fd) const;
		
		virtual bool		add_virtual_server(const IServer& other) = 0;
		virtual bool		operator==(const IServer& other) const;
};

std::ostream&	operator<<(std::ostream& ostream, AServerDispatchSwitch& srv);

#endif