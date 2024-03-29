/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/07 09:51:21 by dsilveri          #+#    #+#             */
/*   Updated: 2023/09/27 21:32:47 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>

#include "ConfigsData.hpp"
#include "Messenger.hpp"
#include "EventLoop.hpp"
#include "Connections.hpp"
#include "EventDemux.hpp"

class Server
{
	private:
		std::vector<std::string>			_serverEndpoints;
		std::map<int, struct sockaddr_in>	_serversInfo;
		ConfigsData							*_configs;
		Messenger							_messenger;
		EventDemux							_eventDemux;
		Connections							_connections;
		EventLoop							_eventLoop;

		bool		_initServers(std::vector<ServerConfig>& serverConfigs);
		bool		_initEventLoop(void);
		void		_initConnections(void);
		bool		_initEventDemux(void);
		int			_initAndStoreSocketInf(std::string host, std::string port);
		bool		_isServerAlreadyInitialized(std::string host, std::string port);
		void		_addNewServerEndpoint(std::string host, std::string port);
		void		_printIniServerError(std::string host, std::string port);
		void		_printActiveEndpoins(void);
		bool		_isValidPort(std::string port);
		
	public:
		Server(void);
		~Server(void);

		void	setConfigs(ConfigsData *configs);
		void	handleControlC(int signal);
		bool	init(void);
		void	start(void);
};
