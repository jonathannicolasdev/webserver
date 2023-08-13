/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:40:57 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/13 18:00:39 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
//# include "ServerHTTP.hpp"
# include "ServerConfig.hpp"

class ServerHTTP;

class Response
{
	protected:
		std::string		_text;

	public:
		Response(void);    
		virtual ~Response(void);

		//int prepare_response(const Request& req);
		int prepare_response(const ServerHTTP& srv, const Request& req, const ServerConfig& cfg);
		const std::string&	get_response(void) const;

		bool	empty(void) const;
};

#endif
