/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TypeTransitionHandler.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 15:00:53 by dsilveri          #+#    #+#             */
/*   Updated: 2023/08/30 17:30:15 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TypeTransitionHandler.hpp"
#include "iostream"

#define CGI_PATH "/cgi"

TypeTransitionHandler::TypeTransitionHandler(ConfigsData *configs): _configs(configs) {}

TypeTransitionHandler::~TypeTransitionHandler(void) {}

void TypeTransitionHandler::handleEvent(Event *event)
{
	//std::cout << "StateTransitionHandler" << std::endl;

	ServerConfig*	serverConf;
	std::string		cgiName;

	if (event->getOldState() == READ_SOCKET)
	{
		serverConf = _getServerConfig(event, _configs->getServers());

		cgiName = serverConf->getCgiScriptName(event->getReqLinePath());

		if (!cgiName.empty())
		{
			std::cout << "Script name: " << cgiName << std::endl;
			event->cgiExecute(serverConf, cgiName);
			event->setActualState(WRITE_CGI);			
		}
		else
			event->setActualState(WRITE_EVENT);
	}
	else if (event->getOldState() == WRITE_EVENT)
	{
		event->setAsFinished();
		event->setActualState(READ_SOCKET);
	}
	else if (event->getOldState() == WRITE_CGI)
	{
		if (event->getCgiExitStatus() != NO_EXIT_STATUS)
			event->setActualState(WRITE_EVENT);
		else
			event->setActualState(READ_CGI);
	}
	else if (event->getOldState() == READ_CGI)
		event->setActualState(WRITE_EVENT);
}

EventType TypeTransitionHandler::getHandleType(void)
{
	return (TYPE_TRANSITION);
}


ServerConfig* TypeTransitionHandler::_getServerConfig(Event *event, std::vector<ServerConfig>& serverConfigs)
{
	std::vector<ServerConfig>::iterator	it;
	std::string							ipReq;
	std::string							portReq;
	std::string							hostReq;
	ServerConfig						*serverConf;
	
	ipReq = event->getIp();
	portReq = event->getPort();
	hostReq = event->getReqHost();
	serverConf = NULL;
	for (it = serverConfigs.begin(); it != serverConfigs.end(); it++)
	{
		if (!ipReq.compare(it->getIp()) && !portReq.compare(it->getPort()))
		{
			if (!hostReq.compare(it->getServerName()))
			{
				serverConf = &(*it);
				break ;
			}
			else if (serverConf == NULL)
				serverConf = &(*it);
		}
	}
	return (serverConf);
}
