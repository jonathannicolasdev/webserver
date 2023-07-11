/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServerCluster.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 21:12:22 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/22 21:13:09 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AServerCluster.hpp"

int	AServerCluster::_counter = 0;

int	AServerCluster::generate_id(void)
{
	return (this->_counter++);
}

AServerCluster::AServerCluster(): _id(generate_id()), _status(CLU_IDLE), _pollfd(0)
{
	std::cout << "AServerCluster constructor" << std::endl;
}

bool	AServerCluster::contains(IServer *srv) const
{
	return (std::find(_servers.begin(), _servers.end(), srv) != _servers.end())
}

IServer			*AServerCluster::find_owner(int client_fd) const
{
	std::vector<IServer *>::iterator	it;

	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
		if ((*it)->is_serving(client_fd))
			return (it);
	return (nullptr);
}
