/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:42:23 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/26 20:59:37 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void) {}
Response::~Response(void) {}

int Response::prepare_response(const Request& req)
{
	(void)req;
	std::cout << "Preparing Response." << std::endl;
	return (0);
}
