/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_utils.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/09 03:14:08 by marvin            #+#    #+#             */
/*   Updated: 2023/08/09 03:14:08 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_UTILS_HPP
# define WEBSERV_UTILS_HPP

# include <unistd.h>
# include <iostream>
# include <ctime>
# include <string>
# include <signal.h>
# include <limits.h>

# define UNUSED(o) (void)(o)

bool		is_all_digits(const std::string& s);
void		gen_timestamp(std::string& ret);
std::string get_working_path(void);

#endif