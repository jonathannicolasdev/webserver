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
# include <cstdlib>

# include "Logger.hpp"
# include "webserv_utils.hpp"
#include "DataPart.hpp"

// Relatively passive container for request data
class Request
{
	private:
		std::string				_method_str;

		std::string				_raw_request;
		std::string				_raw_request_line;
		std::string				_raw_header;

		bool								header_is_parsed;
		std::map<std::string, std::string>	header;
		size_t								header_offset;
		std::string							path;
		std::string							query;
		std::string							protocol;
		std::string							body;
		size_t								content_length;
		std::string							content_length_str;
		bool								is_multipart;
		std::string							boundary;

		// Takes the raw request received from constructor, split the header and body
		// and split the header as a map to put in the header attribute.
		int				process_header(void);
		int				process_body(void);

	public:
		Request(void);
		Request(const std::string& raw_request);
		~Request(void);
		
		int					process_raw_request(void);

		size_t				length(void) const;
		const std::string&	get_method(void) const;
		bool				is_empty(void);
		bool				is_header_parsed(void) const;
		size_t				get_header_length(void) const;
		const std::string&	get_path() const;
		const std::string&	get_query() const;
		const std::string&  get_body() const;
		const std::string&	get_raw_request() const;
		size_t				get_content_length() const;
		const std::string&	get_content_length_str() const;

		bool 				getMultiformFlag(void) const;
		bool 				processMultiform(void) ;
		const std::string&	getBoundary() const;
		
		// Returns the header element at given key.
		const std::string&	operator[](const std::string& key) const;// access header tag values by indexing Request instance like a map. Returns NULL if not tag not in header, returns a string otherwise.

		// Push raw request from client inside Request instance.
		Request&			append(const char *req_buff, size_t n);
		void 				extract_multipart(std::vector<DataPart>& dataparts) const;
};

std::ostream&			operator<<(std::ostream&, const Request& req);

#endif
