/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:40:57 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/26 20:52:25 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Request.hpp"

class Response
{
	private:
		/* data */
	public:
		Response(void);    
		~Response(void);

		int prepare_response(const Request& req);
};

#endif
