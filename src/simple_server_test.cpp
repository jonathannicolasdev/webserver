/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simple_server_test.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/04 01:40:16 by marvin            #+#    #+#             */
/*   Updated: 2023/07/04 01:40:16 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#include "Logger.hpp"
#include "ServerHTTP.hpp"
#include <iostream>

/*
int	map_test()
{
//	ServerHTTP	srv;
	std::map<int, std::string>	mappy;

	mappy[1] = "wow";
	mappy[32] = "hingh!";

	try {
		assert (mappy.find(32) != mappy.end());
		std::cout << "mappy[31] : " << mappy[32] << std::endl;
	}
	catch (std::exception &e) {
		std::cerr << "mappy[32] lookup failed" << std::endl;
	}
//	std::cout << "srv port : " << srv.get_port() << std::endl;
	return (0);
}
*/

//ServerHTTP	*g_srv_ref = NULL;


int	main()
{
	t_webs	*webs = get_webserv_main_struct();

	signal(SIGINT, webs_sigint_handler);

	///// Logger init
	std::string	log_filepath = "logs/webserv.log";
	Logger::init(&log_filepath);
	Logger::log(LOG_DEBUG, "Web Server starts ....... rrrrrrrright now.");


	///// Server init
	//ServerHTTP	srv("", PORT_HTTP, "./www");
	ServerHTTP	srv("Les Jamboni Sapristi inc.", "", 80, "./www/");
	webs->srv = &srv;

	///// Server bind and start
	if (srv.bind_server() < 0)
//	{
//		std::cerr << "Binding server failed hard" << std::endl;
		return (1);
//	}
	std::cout << srv << std::endl;
	if (srv.start(true) < 0)
//	{
//		std::cerr << "Starting server failed hard" << std::endl;
		return (1);
//	}
	Logger::close();
	return (0);
}
