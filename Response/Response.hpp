/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:40:57 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/09 19:44:16 by iamongeo         ###   ########.fr       */
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
	private:
		std::string		_text;

	public:
		Response(void);    
		~Response(void);

		//int prepare_response(const Request& req);
		const int prepare_response(const ServerHTTP& srv, const Request& req, const ServerConfig& cfg) ;

		const std::string&	get_response(void) const;
};

#endif
