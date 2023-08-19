/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 23:07:58 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/18 23:07:58 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>

# include "Logger.hpp"
//# include "webserv.hpp"
#include "DataPart.hpp"

enum e_method
{
	NULL_M,
	GET_M,
	POST_M,
	DELETE_M,
	PUT_M
};

// Relatively passive container for request data
class Request
{
	private:
		enum e_method			_method;
		std::string				_method_str;

		std::string				_raw_request;
		std::string				_raw_request_line;
		std::string				_raw_header;
//		std::string				_raw_body;

		std::map<std::string, std::string>	header;// split and parsed request header.
		std::string							path;
		std::string							query;
		std::string							protocol;
		std::string							body;
		bool								is_multipart;
		std::string							boundary;

		// Takes the raw request received from constructor, split the header and body
		// and split the header as a map to put in the header attribute.
		int				process_request_line(void);
		int				process_header(void);
		int				process_body(void);//const std::string& raw_header);

	public:
		Request(void);
		Request(const std::string& raw_request);
		~Request(void);
		
		int				process_raw_request(void);//const std::string& raw_request);

		size_t			length(void) const;
		const std::string&	get_method(void) const;
		bool			is_method(enum e_method method) const;
		bool			is_empty(void);
		const std::string&	get_path() const;
		const std::string&  get_body() const;
		const std::string&	get_raw_request() const;
		bool 				getMultiformFlag(void) const;
		bool 				processMultiform(void) ;
		std::string			getBoundary() const;
		// Returns the header element at given key.
//		const std::string*	operator[](const std::string& key) const;// access header tag values by indexing Request instance like a map. Returns NULL if not tag not in header, returns a string otherwise.
		const std::string&	operator[](const std::string& key) const;// access header tag values by indexing Request instance like a map. Returns NULL if not tag not in header, returns a string otherwise.
		Request&			operator<<(char *req_buff);// push raw request from client inside Request instance.
		
		std::vector<DataPart> extract_multipart() const;


};

std::ostream&			operator<<(std::ostream&, const Request& req);

#endif
