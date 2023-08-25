/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 04:46:52 by marvin            #+#    #+#             */
/*   Updated: 2023/07/06 04:46:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSRV_LOGGER_HPP
# define WEBSRV_LOGGER_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <ctime>

# define LOGFILE_DEFAULT "logs/webs.log"

# define LOG_RESET_COLOR "\033[0m"
# define LOG_DEBUG_COLOR "\033[1;37m"
# define LOG_WARNING_COLOR "\033[1;33m"
# define LOG_ERROR_COLOR "\033[1;31m"
# define LOG_CRITICAL_COLOR "\033[1;41;31m"

enum e_log_level
{
	LOG_DEBUG,
	LOG_WARNING,
	LOG_ERROR,
	LOG_CRITICAL
};

class Logger
{
	protected:

		static bool				_is_init;
		static std::ofstream	_logfile;

		static int	_log_debug(const std::string &msg);//, bool print);
		static int	_log_warning(const std::string &msg, bool print);
		static int	_log_error(const std::string &msg, bool print);
		static int	_log_critical(const std::string &msg, bool print);

	public:

		static int	init(const std::string& log_filepath);
		static int  log(enum e_log_level lvl, const std::string& msg, bool print);
		static int  log(enum e_log_level lvl, const std::string& msg);
		static int  log(const std::string& msg);
		static void	close(void);
};

#endif