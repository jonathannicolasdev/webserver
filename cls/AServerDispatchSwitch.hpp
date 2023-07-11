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

#ifndef A_SERVER_DISPATCH_SWITCH
# define A_SERVER_DISPATCH_SWITCH

# include <unistd.h>
# include <sys/event.h>
# include <sys/time.h>

# include "Logger.hpp"
# include "IServer.hpp"
# include "AServerReactive.hpp"


# define MAX_PENDING_CONN 128
/*
enum e_react_event
{
	EVNT_ACCEPTED_CONNECTION = 0b1,
//	EVNT_SENT_RESPONSE = 0b10,
	EVNT_SRV_OPEN = 0b100,
	EVNT_SRV_CLOSE = 0b1000,
	EVNT_EXCEPTION_OCCURED = 0x8000000// highest int32 bit
};
*/
//typedef int	(*t_react_callback)(IServer& srv, enum e_react_event event);

enum e_conn_status
{
	CLT_ACEPTED,
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

// This abstract class provides child servers their networking methods.
// After calling bind_server()that creates the network socket and binds it
// to the port given as argument, calling start() will make it listen to new 
// connections. Once the first client connects its socket descriptor is put in
// the t_clt_conn client connection struct where the state of the clients connection
// will be tracked until they disconnect or the server kicks them out for innactivity.
// Server connections are kept alive if instanciated with 
class	AServerDispatchSwitch: public AServerReactive
{
	private:
		int	_init_macos_event_listener(void);
		int	_init_linux_event_listener(void);

		int	_validate_ready_start(void);

	protected:

		//  Uses hash map to store client's connection state info
		// with connection fd (returned by accept()) for key
		// and tracking client activity on site with the connection 
		// state struct defined above. Once the client
		std::map<int, t_clt_conn>	_active_connections;

		int		_subscribed_events = 0;// ORed set of enums from e_react_event
		bool	_keep_alive;
		int		_conn_timout;

		int				_pollfd;// will be kqueue fd on MacOS;
		std::time_t		_srv_start_time;

		AServerDispatchSwitch(uint16_t _port, bool _close_rqst, bool _is_running,
			enum e_server_status_codes _status, bool conn_persistance, int conn_timout=600);
		~AServerDispatchSwitch();

			
		int		bind_server();
		int		start(bool self_managed);
		void	stop();
		void	disconnect(int clt_fd);
		void	disconnect_all(void);
		void	switch_connection_persistance(void);
};

#endif