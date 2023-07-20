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
	this->nb_watched = 0;
	return (0);
}

int
__EventListener::close_poll(void)
{
	if (this->_pollfd < 3)
		return (Logger::log(LOG_WARNING, "Trying to close poll but none currently active."));
	close(this->_pollfd);
	this->nb_watched = 0;
	this->_pollfd = 0;
	return (0);
}

int
__EventListener::poll_new_socket(int sockfd)
{
	std::cout << "polling new socket with sockfd : " << sockfd << std::endl;
	if (this->_pollfd < 3)
		return (Logger::log(LOG_WARNING, "Trying to poll new socket but no polling mechanism is initialized."));
	if (sockfd < 3 || sockfd >= MAX_CONCUR_POLL)
		return (Logger::log(LOG_WARNING, "Trying to poll out of bounds fd"));
	
#ifdef __APPLE__
//	if (this->_events[sockfd].ident > 2)
//		return (Logger::log(LOG_WARNING, "Trying to poll a socket twice"));
//	EV_SET(&this->_events[sockfd], sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	EV_SET(&this->_new_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
//	if ((nb_events = kevent(this->_pollfd, this->_events, MAX_CONCUR_POLL,
	if (kevent(this->_pollfd, &this->_new_event, 1, NULL, 0, NULL) < 0)
		return (Logger::log(LOG_ERROR, "Failed to poll new socket"));
#elif __linux__
//	if (this->_events[sockfd].data.fd > 2)
//		return (Logger::log(LOG_WARNING, "Trying to poll a socket twice"));
    this->_new_event.data.fd = sockfd;
    this->_new_event.events = EPOLLIN;
	if (epoll_ctl(this->_pollfd, EPOLL_CTL_ADD, sockfd, &this->_new_event) < 0)
		return (Logger::log(LOG_ERROR, "Failed to poll new socket"));
#endif
	this->nb_watched++;
    return (0);
}

int
__EventListener::unpoll_socket(int sockfd)
{
	if (this->_pollfd < 3)
		return (Logger::log(LOG_WARNING, "Trying to remove socket from poll but no polling mechanism is initialized."));
	if (sockfd < 3 || sockfd >= MAX_CONCUR_POLL)
		return (Logger::log(LOG_WARNING, "Trying to remove out of bounds fd from poll"));
#ifdef __APPLE__
	EV_SET(&this->_new_event, sockfd, EVFILT_READ, EV_DELETE, 0, 0, 0);
	if (kevent(this->_pollfd, &this->_new_event, 1, NULL, 0, NULL) < 0)
		return (Logger::log(LOG_ERROR, "Failed to poll new socket"));
#elif __linux__
	//if (this->_events[sockfd].data.fd < 3)
	//	return (0);
	if (epoll_ctl(this->_pollfd, EPOLL_CTL_DEL, sockfd, NULL) < 0)
		return (Logger::log(LOG_ERROR, "Failed to remove socket from poll"));
    //this->_new_event.data.fd = 0;
#endif
	this->nb_watched--;
	return (0);
}

/*
int
__EventListener::poll_new_client(int clientfd)
{
 	if (this->_pollfd < 3)
		return (Logger::log(LOG_WARNING, "Trying to poll new client but no polling mechanism is initialized."));
	if (clientfd < 3 || clientfd >= MAX_CONCUR_POLL)
		return (Logger::log(LOG_WARNING, "Trying to poll out of bounds fd"));
	
#ifdef __APPLE__
	if (this->_events[clientfd].ident > 2)
		return (Logger::log(LOG_WARNING, "Trying to poll a client twice"));
	EV_SET(&this->_events[clientfd], clientfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
#elif __linux__
	if (this->_events[sockfd].data.fd > 2)
		return (Logger::log(LOG_WARNING, "Trying to poll a socket twice"));
    this->_events[clientfd].data.fd = clientfd;
    this->_events[clientfd].events = EPOLLIN | EPOLLRDHUP;
	if (epoll_ctl(this->_pollfd, EPOLL_CTL_ADD, clientfd, &this->_events[clientfd]) < 0)
		return (Logger::log(LOG_ERROR, "Failed to poll new socket"));
#endif
	this->nb_watched++;
    return (0);
}

int
__EventListener::unpoll_client(int clientfd)
{
	if (this->_pollfd < 3)
		return (Logger::log(LOG_WARNING, "Trying to remove client from poll but no polling mechanism is initialized."));
	if (clientfd < 3 || clientfd >= MAX_CONCUR_POLL)
		return (Logger::log(LOG_WARNING, "Trying to remove out of bounds fd from poll"));
#ifdef __APPLE__
	if (this->_events[clientfd].ident < 3)
		return (0);
	EV_SET(&this->_events[clientfd], clientfd, EVFILT_READ, EV_DELETE, 0, 0, 0);
	this->_events[clientfd].ident = 0;
#elif __linux__
	if (this->_events[clientfd].data.fd < 3)
		return (0);
	if (epoll_ctl(this->_pollfd, EPOLL_CTL_DEL, clientfd, NULL) < 0)
		return (Logger::log(LOG_ERROR, "Failed to remove client from poll"));
    this->_events[clientfd].data.fd = 0;
#endif
	this->nb_watched--;
	return (0);
}
*/
void
__EventListener::unpoll_socket_array(int *socketfds, int nb_sockets)
{
    for (int i=0; i < nb_sockets; ++i)
        this->unpoll_socket(socketfds[i]);
}


// timeout value in ms.
// Returns the nb of events that occured and are now stored in the _changes struct array.
int
__EventListener::poll_wait(int timeout)
{
	int	nb_events;

	if (this->_pollfd < 3)
		return (Logger::log(LOG_WARNING, "Trying to poll_wait but no polling mechanism is initialized."));
#ifdef __APPLE__
	struct timespec	ts;
	struct timespec	*ts_p;

	ts_p = NULL;
	if (timeout >= 0)
	{
		ts.tv_sec = timeout / 1000;// timeout in seconds
		ts.tv_nsec = (timeout % 1000) * 1000000;// timeout in nano seconds
		ts_p = &ts;
	}
	std::cout << "poll_wait() waiting for kevent() with timeout of " << timeout << "ms" << std::endl;
	if ((nb_events = kevent(this->_pollfd, NULL, 0,//this->_events, MAX_CONCUR_POLL,
		this->_changes, MAX_CONCUR_POLL, ts_p)) < 0)
		return (Logger::log(LOG_ERROR, std::string("kevent() call failed with error : ") + strerror(errno)));

#elif __linux__
	std::cout << "poll_wait() waiting for epoll_wait()" << std::endl;
	if ((nb_events = epoll_wait(this->_pollfd, this->_changes, MAX_CONCUR_POLL, timeout)) < 0)
		return (Logger::log(LOG_ERROR, std::string("poll_wait() call failed with error : ") + strerror(errno)));

#endif
	return (nb_events);
}

int
__EventListener::get_eventfd(int event_idx) const
{
#ifdef	__APPLE__
    return (this->_changes[event_idx].ident);
#elif	__linux__
    return (this->_changes[event_idx].data.fd);
#endif
    return (-1);
}

int
__EventListener::get_read_size(int event_idx) const
{
#ifdef	__APPLE__
    return (this->_changes[event_idx].data);//this->_changes[event_idx].ident);
#elif	__linux__
    return (0);//(this->_changes[event_idx].data.fd);
#endif
    return (-1);
}

