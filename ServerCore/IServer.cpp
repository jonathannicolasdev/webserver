/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IServer.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 17:48:04 by marvin            #+#    #+#             */
/*   Updated: 2023/07/05 17:48:04 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IServer.hpp"

int
__BaseSocketOwner::get_sockfd(void) const {return (this->_sockfd);}

IServer::IServer(uint16_t _port, bool _close_rqst,
    bool _is_running, bool _is_reactive, bool _is_dispatch_switch, enum e_server_status_codes _status):
    _port(_port), _close_request(_close_rqst), _is_running(_is_running),
    _is_dispatch_switch(_is_dispatch_switch), _is_reactive(_is_reactive), _status(_status)
{
	std::memset(&this->_server_addr, 0, sizeof(this->_server_addr));
	this->_server_addr.sin_family = AF_INET;
	this->_server_addr.sin_port = htons(this->_port);
	this->_server_addr.sin_addr.s_addr = INADDR_ANY;
}

IServer::~IServer(void)
{
}

std::string	&srv_status_to_str(enum e_server_status_codes status,
    std::string &ret_str)
{
    switch (status)
    {
        case (SRV_UNBOUND):
            ret_str = "SRV_UNBOUND";
            break ;
        case (SRV_IDLE):
            ret_str = "SRV_IDLE";
            break ;
        case (SRV_LISTENING):
            ret_str = "SRV_LISTENING";
            break ;
        case (SRV_BROKEN):
            ret_str = "SRV_BROKEN";
            break ;
    }
    return (ret_str);
}

// Internal protected methods
bool
IServer::is_dispatch_switch(void) const {return (this->_is_dispatch_switch);}

bool
IServer::is_reactive(void) const {return (this->_is_reactive);}
