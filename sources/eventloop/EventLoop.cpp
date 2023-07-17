/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 14:55:41 by dsilveri          #+#    #+#             */
/*   Updated: 2023/07/17 16:00:53 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"

#include "CGIExecuter.hpp"

EventLoop::EventLoop(void): AMessengerClient(NULL) {}

EventLoop::EventLoop(const EventLoop &src) {}

EventLoop::~EventLoop(void) {}

void EventLoop::registerEventHandler(IEventHandler *eventHandler)
{
	EventType type;
	
	type = eventHandler->getHandleType();
	_handlers.insert(std::make_pair(type, eventHandler));
}

void EventLoop::unregisterEventHandler(IEventHandler *eventHandler)
{
	_handlers.erase(eventHandler->getHandleType());
}

void EventLoop::handleEvents(void)
{
	Event	*event;
	while (!_eventQueue.empty())
	{		
		event = _eventQueue.front();
		_handleEvent(event);
		_eventQueue.pop();
		_handleEventStates(event);
	}
}

ClientID EventLoop::getId(void)
{
	return (EVENTLOOP_ID);
}

void EventLoop::receiveMessage(Message *msg)
{
	MessageType	type;
	int			fd;

	type = msg->getType();
	fd = msg->getFd();
	if (type == EVENT_READ_TRIGGERED)
		_registerReadEvent(fd);
	else if (type == EVENT_WRITE_TRIGGERED)
		_registerWriteEvent(fd);
}

void EventLoop::_handleEvent(Event *ev)
{
	std::map<EventType, IEventHandler*>::iterator it;
	it = _handlers.find((EventType)ev->getState());
	if (it != _handlers.end())
		it->second->handleEvent(ev);
}


Event* EventLoop::_getEventFromMap(int fd)
{
	std::map<int, Event*>::iterator	it;

	it = _eventMap.find(fd);
	if (it != _eventMap.end())
		return (it->second);
	return (NULL);
}

void EventLoop::_addEventToMap(Event *event)
{
	int	fd;

	fd = event->getFd();
	_eventMap.insert(std::make_pair(fd, event));
}

void EventLoop::_addEventToQueue(Event *event)
{
	_eventQueue.push(event);
}

void EventLoop::_registerReadEvent(int fd)
{
	Event*	event;

	event = _getEventFromMap(fd);
	if (!event)
	{
		event = new Event(fd, READ_EVENT);
		_addEventToMap(event);
		sendMessage(new Message(CONNECTIONS_ID, fd, CONNECTION_PAUSE_TIMER));
	}
	if (event->getState() == READ_EVENT || event->getState() == CGI_EVENT)
		_addEventToQueue(event);
}

void EventLoop::_registerWriteEvent(int fd)
{
	Event*	event;

	event = _getEventFromMap(fd);
	if (event && event->getState() == WRITE_EVENT)
		_addEventToQueue(event);
}

void EventLoop::_deleteEvent(int fd)
{
	Event*	event;

	event = _getEventFromMap(fd);
	if (event)
	{
		_eventMap.erase(fd);
		delete event;	
	}
}

void EventLoop::_removeEventFromMap(int fd)
{
	Event*	event;

	event = _getEventFromMap(fd);
	if (event)
		_eventMap.erase(fd);	
}

void EventLoop::_handleEventStates(Event *event)
{
	int fd;

	fd = event->getFd();

	if (event->isEventTimeout())
	{
		std::cout << "TIMEOUT CONEXÃO" << std::endl;
		_deleteEvent(event->getFd());
		sendMessage(new Message(EVENTDEMUX_ID, fd, EVENT_REMOVE));
		sendMessage(new Message(CONNECTIONS_ID, fd, CONNECTION_REMOVE));
		return ;
	}

	if (event->getState() == READ_EVENT_COMPLETE)
	{
		event->setState(WRITE_EVENT);
		sendMessage(new Message(EVENTDEMUX_ID, fd, EVENT_CHANGE_TO_WRITE));
	}
	else if (event->getState() == WRITE_EVENT_COMPLETE)
	{
		if (event->isConnectionClose())
		{
			sendMessage(new Message(EVENTDEMUX_ID, fd, EVENT_REMOVE));
			sendMessage(new Message(CONNECTIONS_ID, fd, CONNECTION_REMOVE));		
		}
		else
		{
			sendMessage(new Message(EVENTDEMUX_ID, fd, EVENT_CHANGE_TO_READ));
			sendMessage(new Message(CONNECTIONS_ID, fd, CONNECTION_RESTART_TIMER));
		}
		_deleteEvent(event->getFd());
	}
	else if(event->getState() == CGI_EVENT)
	{
		if (event->getCgiFd() == -1)
		{
			event->setCgiEx(new CGIExecuter());
			std::cout << "FD CGI: " << event->getCgiFd() << std::endl;
			_eventMap.insert(std::make_pair(event->getCgiFd(), event));
			_eventQueue.push(event);
			sendMessage(new Message(EVENTDEMUX_ID, event->getCgiFd(), EVENT_ADD_NEW));
		}
	}
	else if(event->getState() == CGI_EVENT_COMPLETE)
	{
		std::cout << "CGI completo" << std::endl;
		event->setState(WRITE_EVENT);
		_removeEventFromMap(event->getCgiFd());
		sendMessage(new Message(EVENTDEMUX_ID, event->getCgiFd(), EVENT_REMOVE));
		sendMessage(new Message(EVENTDEMUX_ID, fd, EVENT_CHANGE_TO_WRITE));
	}
	else if (event->getState() == CLOSED_EVENT)
	{
		_deleteEvent(event->getFd());
		sendMessage(new Message(EVENTDEMUX_ID, fd, EVENT_REMOVE));
		sendMessage(new Message(CONNECTIONS_ID, fd, CONNECTION_REMOVE));
	}
}
