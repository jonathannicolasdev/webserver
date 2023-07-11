/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServerReactive.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/04 20:10:57 by marvin            #+#    #+#             */
/*   Updated: 2023/07/04 20:10:57 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef A_SERVER_REACTIVE
# define A_SERVER_REACTIVE

# include "IServer.hpp"

enum e_react_event
{
	EVNT_ACCEPTED_CONNECTION = 0b1,
	EVNT_SENT_RESPONSE = 0b10,
	EVNT_SRV_OPEN = 0b100,
	EVNT_SRV_CLOSE = 0b1000,
	EVNT_EXCEPTION_OCCURED = 0x8000000// highest int32 bit = (1 << 31)
};

typedef int	(*t_react_callback)(IServer& srv, enum e_react_event event, int client_fd);


// Inheriting this abstract class allowes the server to react to events with callbacks.
// The concrete server class 
class	AServerReactive: public IServer
{
	protected:

		std::map<enum e_react_event, t_react_callback> _callbacks;

		int	register_react_callback(enum e_react_event event, t_react_callback cb);		
		int	react(enum e_react_event event);

		AServerReactive(uint16_t _port, bool _close_rqst, bool _is_running, enum e_server_status_codes _status);
};

#endif