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


class ErrorResponse: public Response
{
	private:
		const ServerHTTP&		_srv;
		const Request&			_req;
		const ServerConfig&		_cfg;

		bool	_prepare_default_201_error(void);
		bool    _prepare_default_404_error(void);
		bool    _prepare_default_500_error(void);
		bool	_prepare_default_413_error(void);
		void    _prepare_default_hardcoded_500_error(void);
		bool	__prepare_error(const std::string& dir_path, int error_code);

	public:
		ErrorResponse(const ServerHTTP& srv, const Request& req, const ServerConfig& cfg);
		~ErrorResponse(void);

		static const std::map<int, std::string>	status_msgs;

		int prepare_response(int error_code);

//		const std::string&	get_response(void) const;
};

#endif