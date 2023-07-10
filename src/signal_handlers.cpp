/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 16:50:39 by iamongeo          #+#    #+#             */
/*   Updated: 2023/07/09 16:58:28 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void	webs_sigint_handler(int signum)
{
    t_webs  *webs;

	(void)signum;
    webs = get_webserv_main_struct();
	std::cerr << "EXIT WITH SIGINT !!" << std::endl;
	if (webs->is_init && webs->srv)
	{
		webs->srv->stop();
		webs->flag_global_termination = true; 
	}
	exit(1);
}
