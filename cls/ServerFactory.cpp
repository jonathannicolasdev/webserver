/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFactory.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 21:21:46 by marvin            #+#    #+#             */
/*   Updated: 2023/07/03 21:21:46 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerFactory.hpp"

// Creates and builds a new instance of a Server of the type mentioned as parameter
// 
IServer *ServerFactory::create_server(enum e_srv_type_srv srv_type, uint16_t port,
    const std::string& rootdir)
{
    std::cout << "Creating server from ServerFactory. type " << srv_type << std::endl;
    switch (srv_type) {
        case SRVF_HTTP:
            return (new ServerHTTP(
                (!port) ? (PORT_HTTP):(port),
                (!rootdir) ? ("./"):(rootdir)));

//  Add more cases with their 
        default:
            std::cerr << "Server factory failed to create server of type code "
                << srv_type << " because : Not implemented." << std::endl;
                return (NULL);
    }
}

IServer *ServerFactory::create_server_from_cfg(const std::map<std::string, std::string> &cfg)
{
    IServer *new_server;

    if (!cfg)
        return (NULL);
    new_server = NULL;    

    // TODO : implement config file parsing to build config map to pass to this function.
    std::cerr << "Called unimplemented ServerFactory create_server_from_cfg()." << std::endl;
    return (new_server);
}