/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIAgent.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 20:47:22 by marvin            #+#    #+#             */
/*   Updated: 2023/08/11 20:47:22 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIAGENT_HPP
# define CGIAGENT_HPP

# include <iostream>
# include <string>
# include <vector>

struct CGIConfig
{
	std::string		host;
	std::string		content_type;
	std::string		;
	std::string		host;
	std::string		host;
	std::string		host;
	std::string		host;
};


class CGIAgent
{
	private:

		std::vector<std::string>	env_str;
		std::vector<char*>			env_vect;

	public:
		CGIAgent(void);
		CGIAgent(const Request& req);
		~CGIAgent(void);

};

#endif