/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 11:51:21 by dsilveri          #+#    #+#             */
/*   Updated: 2023/09/21 12:31:38 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "Timer.hpp"
#include "configs.hpp"

Connection::Connection(int fd):
	_fd(fd),
	_keepAliveTimeout(KEEP_ALIVE_TIMEOUT_SEC),
	_lastRequestTime(Timer::getActualTimeStamp()),
	_status(TIMER_ACTIVE)
{}

Connection::~Connection(void)
{
	//std::cout << "remove connection: " << _fd << std::endl;
	close(_fd);
}

int Connection::getFd(void)
{
	return (_fd);
}

bool Connection::isKeepAliveTimeout(void)
{
	if (_status == TIMER_ACTIVE)
		return (Timer::isTimeoutExpired(_lastRequestTime, _keepAliveTimeout));
	else
		return (false);
}

void Connection::resetKeepAliveTimeout(void)
{
	_lastRequestTime = Timer::getActualTimeStamp();
}

void Connection::startTimer(void)
{
	this->resetKeepAliveTimeout();
	_status = TIMER_ACTIVE;
}

void Connection::pauseTimer(void)
{
	_status = TIMER_PAUSED;
}

// DEBUG
void Connection::showDataConnection(void)
{
	std::cout << "------------ Connection ------------" << std::endl;
	std::cout << "fd: " << _fd << std::endl;
	std::cout << "keep-alive: " << _keepAliveTimeout << std::endl;
	std::cout << "last request: " << _lastRequestTime << std::endl;
}
