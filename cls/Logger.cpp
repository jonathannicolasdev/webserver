/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 04:55:00 by marvin            #+#    #+#             */
/*   Updated: 2023/07/06 04:55:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Logger.hpp"

bool			Logger::_is_init = false;
std::ofstream	Logger::_logfile;

std::string	log_level_to_string(enum e_log_level lvl)
{
	if (lvl == LOG_DEBUG)
		return ("Debug");
	else if (lvl == LOG_WARNING)
		return ("Warning");
	else if (lvl == LOG_ERROR)
		return ("Error");
	else if (lvl == LOG_CRITICAL)
		return ("CRITICAL");
	else
		std::cerr << "Invalid log level" << std::endl;
	return ("");
}

int	Logger::init(const std::string *log_filepath)
{
	std::string	fp;

	if (Logger::_is_init)
	{
		std::cerr << "Logger already initialized" << std::endl;
		return (-1);
	}

	if (!log_filepath)
		fp = LOGFILE_DEFAULT;
	else
		fp = *log_filepath;

	Logger::_logfile.open(fp.c_str(), std::ofstream::out | std::ofstream::app);
	if (!Logger::_logfile.is_open())
	{
		std::cerr << "Failed to open logger file : " << fp << std::endl;
		return (-1);
	}
	Logger::_is_init = true;
	return (0);
}

void	Logger::close(void)
{
	_logfile.close();
	std::cerr << "Logger closed" << std::endl;
}

int	Logger::_log_debug(const std::string& msg)//, bool print)
{
	std::string		log_msg;

	gen_timestamp(log_msg);
	log_msg += " :: " + log_level_to_string(LOG_DEBUG) + " :: " + msg;

	std::cerr << LOG_DEBUG_COLOR << log_msg << LOG_RESET_COLOR << std::endl;
	return (0);
}

int	Logger::_log_warning(const std::string& msg, bool print)
{
	std::string		log_msg;

	gen_timestamp(log_msg);
	log_msg += " :: " + log_level_to_string(LOG_WARNING) + " :: " + msg;

	if (print)
		std::cerr << LOG_WARNING_COLOR << log_msg << LOG_RESET_COLOR << std::endl;
	_logfile << log_msg << std::endl;
	return (-1);
}

int	Logger::_log_error(const std::string& msg, bool print)
{
	std::string		log_msg;

	gen_timestamp(log_msg);
	log_msg += " :: " + log_level_to_string(LOG_ERROR) + " :: " + msg;

	if (print)
		std::cerr << LOG_ERROR_COLOR << log_msg << LOG_RESET_COLOR << std::endl;
	_logfile << log_msg << std::endl;
	return (-1);
}

int	Logger::_log_critical(const std::string& msg, bool print)
{
	std::string		log_msg;

	gen_timestamp(log_msg);
	log_msg += " :: " + log_level_to_string(LOG_CRITICAL) + " :: " + msg;

	if (print)
		std::cerr << LOG_CRITICAL_COLOR << log_msg << LOG_RESET_COLOR << std::endl;
	_logfile << log_msg << std::endl;
	return (-1);
}

int  Logger::log(enum e_log_level lvl, const std::string& msg, bool print)
{
	if (!Logger::_is_init)
		Logger::init(NULL);
	if (lvl == LOG_DEBUG)
		return (_log_debug(msg));
	else if (lvl == LOG_WARNING)
		return (_log_warning(msg, print));
	else if (lvl == LOG_ERROR)
		return (_log_error(msg, print));
	else if (lvl == LOG_CRITICAL)
		return (_log_critical(msg, print));
	else
		std::cerr << "Invalid log level" << std::endl;
	return (-1);
}

int  Logger::log(enum e_log_level lvl, const std::string& msg)
{
	return (Logger::log(lvl, msg, true));
}

int  Logger::log(const std::string& msg)
{
	if (!Logger::_is_init)
		Logger::init(NULL);
	return (_log_debug(msg));
}
