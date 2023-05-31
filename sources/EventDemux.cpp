/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventDemux.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/24 12:10:06 by dsilveri          #+#    #+#             */
/*   Updated: 2023/05/31 15:59:20 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventDemux.hpp"

EventDemux::EventDemux(void): AMessengerClient(NULL) {}

EventDemux::EventDemux(int serverFd, struct sockaddr_in address, socklen_t addrlen):
	AMessengerClient(NULL),
	_serverFd(serverFd),
	_address(address),
	_addrlen(addrlen)
{
	_epollFd = epoll_create1(0);
	_addNewEvent(serverFd);
}

EventDemux::EventDemux(const EventDemux &src) {}

EventDemux::~EventDemux(void){}

/*
EventDemux &EventDemux::operator=(const EventDemux &src)
{
	//EventDemultiplexer Copy Assignment Operator
}
*/

void EventDemux::waitAndDispatchEvents(void)
{
	int		numEvents;
	int		eventFd;

	//std::cout << "wait events" << std::endl;
	numEvents = epoll_wait(_epollFd, _events, N_MAX_EVENTS, 1000);
	for (int i = 0; i < numEvents; i++) 
	{
		eventFd = _events[i].data.fd;
		if (eventFd == _serverFd)
		{
			// needs to check if it returns error
			eventFd = accept(_serverFd, (struct sockaddr *) &_address, &_addrlen);
			_addNewEvent(eventFd);
		}
		else 
			sendMessage(new EventMessage(EVENTLOOP_ID, eventFd, _getEventType(_events[i].events)));
		sendMessage(new ConnectionMessage(CONNECTIONS_ID, eventFd));
	}
}

ClientID EventDemux::getId(void)
{
	return (EVENTDEMUX_ID);
}

void EventDemux::receiveMessage(Message *msg)
{
	ConnectionMessage	*connMsg;
	EventMessage		*eventMsg;

	connMsg = dynamic_cast<ConnectionMessage*>(msg);
	eventMsg = dynamic_cast<EventMessage*>(msg);
	if (connMsg)
		_removeEvent(connMsg->getFd());
	else if (eventMsg)
		_changeEvent(eventMsg->getFd(), (Type)eventMsg->getEvent());
}

void EventDemux::_addNewEvent(int fd)
{
	struct epoll_event	ev;
	int					flags;
		
	ev.data.fd = fd;
    ev.events = EPOLLIN;
	flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
		std::cerr << "Failed to add socket to epoll." << std::endl;
}

void EventDemux::_removeEvent(int fd)
{
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		std::cerr << "Failed to remove event from epoll" << std::endl;
	}	
}

void EventDemux::_changeEvent(int fd, Type eventType)
{
	struct epoll_event	ev;

    ev.data.fd = fd;
    ev.events = _getEventsMask(eventType);
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev) == -1)
		std::cerr << "Failed to modify socket to epoll." << std::endl;
}

Type EventDemux::_getEventType(uint32_t events)
{
	Type evType;

	if (events & EPOLLIN)
		evType = READ;
	else if (events & EPOLLOUT)
		evType = WRITE;
	return (evType);
}

uint32_t EventDemux::_getEventsMask(Type eventType)
{
	uint32_t events;

	events = 0;
	if (eventType == READ)
		events = EPOLLIN;
	if (eventType == WRITE)
		events = EPOLLOUT;
	return (events);
}