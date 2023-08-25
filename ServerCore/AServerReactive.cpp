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

AServerReactive::AServerReactive(uint16_t _port, bool _close_rqst, bool _is_running,
	bool _is_dispatch_switch, enum e_server_status_codes _status):
	IServer(_port, _close_rqst, _is_running, true, _is_dispatch_switch, _status),
	_subscribed_events(0)
{
}

AServerReactive::~AServerReactive(void)
{
}

// event will be valid and will be one single event bit since event is enum e_react_event type.
int	AServerReactive::register_react_callback(enum e_react_event event, t_react_callback cb)
{
	if (!cb)
		return (-1);
	
	this->_callbacks[event] = cb;
	this->_subscribed_events &= event;
	return (0);
}

// clientfd can be ignored depending on event.
int	AServerReactive::react(enum e_react_event event, int clientfd)
{
	std::map<enum e_react_event, t_react_callback>::iterator	it;
	t_react_callback	cb;

	cb = NULL;
	it = this->_callbacks.find(event);
	if (it != this->_callbacks.end())
	{
		cb = this->_callbacks[event];
		return (cb(*this, event, clientfd));
	}
	return (0);
}
