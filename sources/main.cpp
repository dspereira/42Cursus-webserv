/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcandeia <dcandeia@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/13 11:52:16 by dsilveri          #+#    #+#             */
/*   Updated: 2023/09/12 20:35:53 by dcandeia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configs.hpp"
#include "ConfigsData.hpp"
#include "Server.hpp"
#include <csignal>

#include "Signals.hpp"

#define DEFAULT_CONFIG_PATH		"default_configs/default.conf"

bool	initConfigs(const char *filename, ConfigsData &data)
{
	try
	{
		Configs	cfg(filename);
		data.setupConfigs(cfg.getFileContentMap());
	}
	catch(const std::exception& e)
	{
		// Criar mensagens apropriadas
		// std::cerr << BOLDRED << "Webserv: Invalid Configuration" << RESET << std::endl;
		std::cerr << BOLDRED << e.what() << RESET << std::endl;
		return (false);
	}
	return (true);
}

int main(int argc, char **argv)
{
	ConfigsData	confData;
	Signals::init();

	if (argc != 2)
	{
		if (!initConfigs(DEFAULT_CONFIG_PATH, confData))
			return (-1);
	}
	else
	{
		if (!initConfigs(argv[1], confData))
			return (-1);
	}

	Server		server;
	server.setConfigs(&confData);
	if (server.init())
		server.start();
	return (0);
}
