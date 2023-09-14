/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connections.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 11:51:32 by dsilveri          #+#    #+#             */
/*   Updated: 2023/09/14 10:51:22 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connections.hpp"

#include "Timer.hpp"

Connections::Connections(void): AMessengerClient(NULL), _lastUpdateTime(0) {}

Connections::~Connections(void)
{
	_removeAllConnections();
	//std::cout << "~Connections" << std::endl;
}

void Connections::updateAllConnections(void)
{
	std::map<int, Connection *>::iterator it;
	std::map<int, Connection *>::iterator itBuff;

	if (Timer::isTimeoutExpired(_lastUpdateTime, 1))
	{
		it = _activeConnects.begin();
		while (it != _activeConnects.end())
		{
			if (it->second->isKeepAliveTimeout())
			{
				sendMessage(new Message(EVENTDEMUX_ID, it->second->getFd(), EVENT_REMOVE));
				itBuff = it;
				itBuff++;
				_removeConnection(it);
				it == itBuff;
				break;
			}

			it++;
		}
		_lastUpdateTime = Timer::getActualTimeStamp();
	}
}

ClientID Connections::getId(void)
{
	return (CONNECTIONS_ID);
}

void Connections::receiveMessage(Message *msg)
{
	MessageType	type;
	int			fd;

	type = msg->getType();
	fd = msg->getFd();
	if (type == CONNECTION_ADD_NEW)
		_addNewConnection(fd);
	else if (type == CONNECTION_REMOVE)
		_removeConnection(fd);
	else if (type == CONNECTION_PAUSE_TIMER)
		_pauseKeepAliveTimer(fd);
	else if (type == CONNECTION_RESTART_TIMER)
		_restartKeepAliveTimer(fd);
	else if (type == CONNECTION_RESET_TIMER)
		_resetKeepAliveTimer(fd);
}

// DEBUG
void Connections::showConnections(void)
{
	std::map<int, Connection *>::iterator it;

	for(it = _activeConnects.begin(); it != _activeConnects.end(); it++)
		it->second->showDataConnection();
}

Connection * Connections::_getConnection(int fd)
{
	std::map<int, Connection *>::iterator it;

	it = _activeConnects.find(fd);
	if (it != _activeConnects.end())
		return (it->second);
	return (NULL);
}

void Connections::_removeAllConnections(void)
{
	std::map<int, Connection *>::iterator it;

	for(it = _activeConnects.begin(); it != _activeConnects.end(); it++)
		delete it->second;
}

void Connections::_removeConnection(std::map<int, Connection *>::iterator it)
{
	delete it->second;
	_activeConnects.erase(it);
}

void Connections::_removeConnection(int fd)
{
	std::map<int, Connection *>::iterator it;

	it = _activeConnects.find(fd);
	if (it != _activeConnects.end())
	{
		delete it->second;
		_activeConnects.erase(it);
	}
}

void Connections::_addNewConnection(int fd)
{
	try 
	{
		_activeConnects.insert(std::make_pair(fd, new Connection(fd)));
	}
	catch (const std::exception& e)
	{
		sendMessage(new Message(EVENTDEMUX_ID, fd, EVENT_REMOVE));
	}
}

void Connections::_pauseKeepAliveTimer(int fd)
{
	Connection * con;

	con = _getConnection(fd);
	if (con)
		con->pauseTimer();
}

void Connections::_restartKeepAliveTimer(int fd)
{
	Connection * con;

	con = _getConnection(fd);
	if (con)
		con->startTimer();
}

void Connections::_resetKeepAliveTimer(int fd)
{
	Connection * con;

	con = _getConnection(fd);
	if (con)
		con->resetKeepAliveTimeout();
}
