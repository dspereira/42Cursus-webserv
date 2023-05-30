/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcandeia <dcandeia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/13 11:52:16 by dsilveri          #+#    #+#             */
/*   Updated: 2023/05/04 13:39:35 by dcandeia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/poll.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>

#include "Configs.hpp"
#include "ConfigsData.hpp"
#include "RequestParser.hpp"
#include "RequestData.hpp"

bool	initConfigs(const char *filename, ConfigsData &data)
{
	std::string	key, value;

	try
	{
		Configs	cfg(filename);
		while (cfg.getNextConfig(key, value))
			data.addNewConfigs(key, value);
		// std::cout << "listen:   " << data.getListen() << std::endl;
		// std::cout << "ServName: " << data.getServerName() << std::endl;
		// std::cout << "Root:     " << data.getRoot() << std::endl;
		// std::cout << "Index:    " << data.getIndex() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (false);
	}
	return (true);
}

/* int main(int ac, char **av)
{
	ConfigsData	data;

	if (ac != 2)
	{
		Terminal::printErrors("Invalid number of Arguments");
		return (1);
	}

	if (!initConfigs(av[1], data))
		return (1);

	return (0);
} */

int main(int ac, char **av)
{
	int	fd1;
	RequestData	data;

	if (ac < 2)
	{
		Terminal::printErrors("Invalid number of Arguments");
		return (1);
	}

	fd1 = open(av[1], O_RDONLY);
	if (fd1 < 0)
	{
		Terminal::printErrors("Invalid Request File");
		return (1);
	}

	try
	{
		RequestParser 										request1(fd1);
		std::string											requestLine;
		std::map<std::string, std::vector<std::string> >	requestHeader;
		std::string											requestBody;

		requestLine = request1.getRequestLine();
		requestHeader = request1.getRequestHeader();
		requestBody = request1.getRequestBody();

		// std::cout << "Line: " << requestLine << std::endl;

		std::cout << "[KEY] | [VALUE]" << std::endl;
		std::map<std::string, std::vector<std::string> >::iterator	it;
		std::vector<std::string>									elements;
		
		for (it = requestHeader.begin(); it != requestHeader.end(); it++)
		{
			elements = (*it).second;
			std::cout << "[" << (*it).first << "] | ";
			for (size_t i = 0; i < elements.size(); i++)
			{
				std::cout << "[" << elements.at(i).c_str() << "]";
				if (i < elements.size() - 1)
					std::cout << " , ";
			}
			std::cout << std::endl;
		}

		// std::cout << requestBody << std::endl;

		data.setRequestLine(requestLine);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}

	std::vector<std::string>	line;
	line = data.getRequestLine();

	std::cout << "---------- Request Line ----------" << std::endl;
	for (size_t i = 0; i < line.size(); i++)
		std::cout << "\'" << line.at(i).c_str() << "\'" << std::endl; 
	std::cout << "----------------------------------" << std::endl;

	return (0);
}
