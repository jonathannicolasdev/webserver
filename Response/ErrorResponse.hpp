/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 16:26:47 by marvin            #+#    #+#             */
/*   Updated: 2023/08/12 16:26:47 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_RESPONSE_HPP
# define ERROR_RESPONSE_HPP

# include <fstream>
# include <sstream>
# include <string>
# include <map>
# include "ServerHTTP.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ServerConfig.hpp"


class Response;

class ErrorResponse: public Response
{
	private:
		const ServerHTTP&		_srv;
		const Request&			_req;
		const ServerConfig&		_cfg;

		void    _prepare_default_hardcoded_500_error(void);
		void	__prepare_error(const std::string& filepath, int error_code);

	public:
		ErrorResponse(const ServerHTTP& srv, const Request& req, const ServerConfig& cfg);
		~ErrorResponse(void);
		
		void	prepare_response(int error_code);
};

#endif