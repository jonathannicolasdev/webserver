/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __EventListener.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 04:19:47 by marvin            #+#    #+#             */
/*   Updated: 2023/07/13 04:19:47 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AServerCluster.hpp"


int
__EventListener::init_poll(void)
{
	if (this->_pollfd > 2)
		return (Logger::log(LOG_WARNING, "Trying to init new poll but polling fd already initialized."));

#ifdef __APPLE__
	if ((this->_pollfd = kqueue()) < 0)
#elif __linux__
	if ((this->_pollfd = epoll_create1(0)) < 0)
#endif
		return (Logger::log(LOG_ERROR, "Failed to initialize polling mechanism."));

	return (0);
}

int
__EventListener::close_poll(void)
{
	if (this->_pollfd < 3)
		return (Logger::log(LOG_WARNING, "Trying to close poll but none currently active."));
	
	close(this->_pollfd);
	return (0);
}

int
__EventListener::poll_new_socket(int sockfd)
{
	if (this->_pollfd < 3)
		return (Logger::log(LOG_WARNING, "Trying to poll new socket but no polling mechanism was initialized."));
	
    EV_SET(change_event, socket_listen_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

    return (0);
}

int
__EventListener::unpoll_socket(int sockfd)
{
    // TO IMPLEMENT
    return (0);
}

int
__EventListener::poll_new_client(int sockfd)
{
    // TO IMPLEMENT
    return (0);
}

int
__EventListener::unpoll_client(int sockfd)
{
    // TO IMPLEMENT
    return (0);
}
