/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServerReactive.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/04 20:12:53 by marvin            #+#    #+#             */
/*   Updated: 2023/07/04 20:12:53 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AServerReactive.hpp"

int	AServerReactive::register_react_callback(enum e_react_event event, t_react_callback cb)
{
	if (!cb)
		return (-1);
	
//	notify_cb = cb;
//	for (int ev=0b1; ev < EVNT_EXCEPTION_OCCURED; ev <<= 1)
//	{
//		if (ev & event)
//			callback[(enum e_reat_event)ev] = cb;
//	}
	callbacks[event] = cb;
	return (0);
}

int	AServerReactive::react(enum e_react_event event)
{
	t_react_callback	cb;

	try {
		cb = callbacks[event];
    }
	catch (const std::exception &e) {
		std::cerr << "No registered callback for event code : " << event << std::endl;
		return (-1);
	}
	return (cb(*this, event));
}

AServerReactive::AServerReactive(uint16_t _port, bool _close_rqst, bool _is_running,
    enum e_server_status_codes _status): 
	IServer(_port, _close_rqst, _is_running, _status)
{
	std::cout << "AServerReactive passthrough constructor" << std:: endl;
}