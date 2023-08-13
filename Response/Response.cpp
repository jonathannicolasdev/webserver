/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:42:23 by iamongeo          #+#    #+#             */
/*   Updated: 2023/08/09 20:35:02 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void) {}
Response::~Response(void) {}

void	_prepare_dummy_response(std::string& text_response)
{
	std::string			header;
	std::string			body;
	std::ostringstream	content_length;
	std::string			response;

	header = "HTTP/1.1 200 OK\r\n";
	header += "Content-type: text/html\r\n";
	
	body +=	"<!DOCTYPE html>\r\n";
	body +=	"<html>\r\n";
	body +=	"	<head>\r\n";
	body +=	"		<title>Webserv Index</title>\r\n";
	body +=	"	</head>\r\n";
	body +=	"	<body>\r\n";
	body +=	"		<h1>Hello World page1</h1>\r\n";
	body +=	"		<img src=\"asdf.jpg\" width=100 height=100>\r\n";
	body +=	"		<a href=\"page2.html\">My beautiful link</a>\r\n";
	body +=	"	</body>\r\n";
	body +=	"</html>\r\n";

	header += "Content-length: ";
	content_length << body.length();
	header += content_length.str();
	header += "\r\n\r\n";
	text_response = header + body;
}


/// REQUIRED METHODS BY SERVER ///////////////
int Response::prepare_response(const ServerHTTP& srv, const Request& req, const ServerConfig& cfg) const
{
	(void)srv;
	(void)req;
	(void)cfg;

	std::cout << " ******************* PATH:" << req.get_path() <<"\n";
	std::cout << " ******************* SERVERCONFIG :"  ;
	cfg.print();
	std::cout << "Preparing Response." << std::endl;
	int locationIndex = cfg.getBestLocationMatch(req.get_path());
	if (locationIndex==-1)
	{
		std::cout << "no location found";
	}
	else{
		LocationConfig bestLocation = cfg.GetLocations().at(locationIndex);
		bestLocation.print();
	}

	// DUMMY RESPONSE. DELETE ME
	//_prepare_dummy_response(_text);
	return (0);
}

const std::string&	Response::get_response(void) const {return (this->_text);}
/// END OF REQUIRED METHODS BY SERVER /////////////
