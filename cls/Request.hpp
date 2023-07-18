/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 22:02:57 by marvin            #+#    #+#             */
/*   Updated: 2023/07/17 22:02:57 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>

enum e_method
{
	NULL_M,
	GET_M,
	POST_M,
	DELETE_M,
	PUT_M
};

// Relatively passive container for request data
class Request:
{
	private:
		enum e_method	_method;

		std::string     _raw_request;
		std::string     _raw_header;

		std::map<std::string, std::string>	header;// split and parsed request header.
		std::string							body;

		// Takes the raw request received from constructor, split the header and body
		// and split the header as a map to put in the header attribute.
		int	process_raw_request(const std::string& raw_request);


	public:
		Request(const std::string& raw_request);
		~Request(void);

		enum e_method	get_method(void) const;
		bool			is_method(enum e_method method) const;

		// Returns the header element at given key.
		const std::string*	operator[](const std::string& key) const;
				
};

#endif