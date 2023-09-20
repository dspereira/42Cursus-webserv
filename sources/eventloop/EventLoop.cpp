/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 14:55:41 by dsilveri          #+#    #+#             */
/*   Updated: 2023/09/20 12:49:31 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include "Signals.hpp"
#include "CgiExec.hpp"

EventLoop::EventLoop(void): AMessengerClient(NULL) {}

EventLoop::~EventLoop(void)
{
	_cleanHandlers();
	_cleanEvents();
}

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
	int		fd;

	while (!_eventQueue.empty())
	{
		fd = _getNextEventFromQueue();
		event = _getEventFromMap(_eventMap, fd);
		if (event)
		{
			_handleEvent(event);
			_changeEventStatus(event);
		}
	}
	_checkIfCgiScriptsFinished();
}

ClientID EventLoop::getId(void)
{
	return (EVENTLOOP_ID);
}

void EventLoop::receiveMessage(Message msg)
{
	MessageType	type;
	int			fd;

	type = msg.getType();
	fd = msg.getFd();
	if (type == EVENT_READ_TRIGGERED)
		_registerReadEvent(fd);
	else if (type == EVENT_WRITE_TRIGGERED)
		_registerWriteEvent(fd);
}

void EventLoop::_handleEvent(Event *ev)
{
	std::map<EventType, IEventHandler*>::iterator	it;
	EventType										type;

	if (ev)
	{
		type = ev->getActualState();
		it = _handlers.find(type);
		if (it != _handlers.end())
			it->second->handleEvent(ev);
	}
}

void EventLoop::_registerReadEvent(int fd)
{
	Event*		event;
	EventType	type;

	event = _getEventFromMap(_eventMap, fd);
	if (!event)
	{
		event = new Event(fd, READ_SOCKET);
		_insertEventToMap(_eventMap, fd, event);
		sendMessage(Message(CONNECTIONS_ID, fd, CONNECTION_PAUSE_TIMER));
	}
	else if (event)
	{
		type = event->getActualState();
		if ((type == READ_SOCKET && event->getFd() == fd)
			|| (type == READ_CGI && event->getCgiReadFd() == fd))
			_addEventToQueue(fd);
		else
		{
			event->setIsStateChange(true);
			event->setActualState(DISCONNECT_EVENT);
		}
	}
}

void EventLoop::_registerWriteEvent(int fd)
{
	Event*		event;
	EventType	type;

	event = _getEventFromMap(_eventMap, fd);
	if (!event)
		return ;
	type = event->getActualState();
	if ((type == WRITE_EVENT && fd == event->getFd())
		|| (type == WRITE_CGI && fd == event->getCgiWriteFd()))
		_addEventToQueue(fd);
}

void EventLoop::_addEventToQueue(int fd)
{
	_eventQueue.push(fd);
}

Event* EventLoop::_getEventFromMap(std::map<int, Event*> &map, int fd)
{
	std::map<int, Event*>::iterator	it;

	it = map.find(fd);
	if (it != map.end())
		return (it->second);
	return (NULL);
}

void EventLoop::_closeTimeoutEvents(void)
{
	std::map<int, Event*>::iterator	it;
	Event							*event;
	int								fd;
	int								cgiReadFd;
	int								cgiWriteFd;

	if (_eventMap.empty())
		return ;
	for (it = _eventMap.begin(); it != _eventMap.end(); it++)
	{
		event = it->second;
		fd = event->getFd();

		if (fd != it->first)
			continue ;
		//cgiReadFd = event->getCgiReadFd();
		//cgiWriteFd = event->getCgiWriteFd();

		if (event->isEventTimeout())
		{
			if (event->getActualState() != WRITE_EVENT)
			{
				event->setStatusCode(REQUEST_TIMEOUT_CODE);
				event->setActualState(TYPE_TRANSITION);
				_addEventToQueue(event->getFd());
			}
		}

		/*if (event->isEventTimeout())
		{
			if (cgiReadFd > 0)
				sendMessage(new Message(EVENTDEMUX_ID, cgiReadFd, EVENT_REMOVE));
			if (cgiWriteFd > 0)
				sendMessage(new Message(EVENTDEMUX_ID, cgiWriteFd, EVENT_REMOVE));
			sendMessage(new Message(EVENTDEMUX_ID, fd, EVENT_REMOVE));
			sendMessage(new Message(CONNECTIONS_ID, fd, CONNECTION_REMOVE));
			_deleteEvent(fd);
		}*/
	}
}

void EventLoop::_checkIfCgiScriptsFinished(void)
{
	std::map<int, Event*>::iterator	it;
	std::vector<int>::iterator		itVect;
	std::vector<int>				elmToRemove;
	Event							*event;
	int								fd;

	for (it = _cgiEventMap.begin(); it != _cgiEventMap.end(); it++)
	{
		fd = it->first;
		if (!_getEventFromMap(_eventMap, fd))
			elmToRemove.push_back(fd);
		else
		{
			event = it->second;
			if (event->isCgiScriptEndend())
			{
				event->setIsStateChange(true);
				event->setActualState(WRITE_EVENT);
				_changeEventStatus(event);
				elmToRemove.push_back(fd);
			}
			else if (CgiExec::isEnded(event))
				event->setCgiScriptEndend(true);
		}
	}
	if (elmToRemove.empty())
		return ;
	for (itVect = elmToRemove.begin(); itVect != elmToRemove.end(); itVect++)
		_cgiEventMap.erase(*itVect);

}

int EventLoop::_getNextEventFromQueue(void)
{
	int fd;

	fd = _eventQueue.front();
	_eventQueue.pop();
	return (fd);
}

void EventLoop::_cleanHandlers(void)
{
	std::map<EventType, IEventHandler*>::iterator it;

	for(it = _handlers.begin(); it != _handlers.end(); it++)
		delete it->second;
}

void EventLoop::_cleanEvents(void)
{
	std::map<int, Event*>::iterator	it;
	Event							*event;
	int								fd;

	for(it = _eventMap.begin(); it != _eventMap.end(); it++)
	{
		fd = it->first;
		event = it->second;
		if (fd != event->getFd())
			it->second = NULL;
	}
	for(it = _eventMap.begin(); it != _eventMap.end(); it++)
	{
		if (it->second)
			delete it->second;
	}	
}

void EventLoop::_changeEventStatus(Event *event)
{
	EventType	type;
	int			fd;
	
	if (!event->isStateChange())
		return ;
	event->setIsStateChange(false);
	type = event->getActualState();
	fd = event->getFd();
	
	if (type == WRITE_CGI)
		_changeToWriteCgi(event);
	if (type == READ_CGI)
		_changeToReadCgi(event);
	if (type == WRITE_EVENT)
		_changeToWriteSocket(event);
	if (type == CLOSE_EVENT)
		_closeEvent(event);
	if (type == DISCONNECT_EVENT)
		_disconnectEvent(event);	
}

void EventLoop::_changeToWriteCgi(Event *event)
{
	int fd;

	if (CgiExec::execute(event) == -1)
	{
		event->setStatusCode(INTERNAL_SERVER_ERROR_CODE);
		event->setActualState(WRITE_EVENT);
	}
	else
	{
		fd = event->getCgiWriteFd();
		_insertEventToMap(_cgiEventMap, event->getFd(), event);
		_insertEventToMap(_eventMap, fd, event);
		sendMessage(Message(EVENTDEMUX_ID, fd, EVENT_ADD_NEW));
		sendMessage(Message(EVENTDEMUX_ID, fd, EVENT_CHANGE_TO_WRITE));
		event->setCgiFdRemoved(fd, false);
	}
	event->setIsStateChange(false);
}

void EventLoop::_changeToReadCgi(Event *event)
{
	int readFd;
	int writeFd;

	readFd = event->getCgiReadFd();
	writeFd = event->getCgiWriteFd();
	if (writeFd > 0 && !event->isCgiWriteFdRemoved())
	{
		sendMessage(Message(EVENTDEMUX_ID, writeFd, EVENT_REMOVE));
		event->setCgiFdRemoved(writeFd, true);
	}
	if (writeFd > 0)
		_eventMap.erase(writeFd);
	_insertEventToMap(_eventMap, readFd, event);
	sendMessage(Message(EVENTDEMUX_ID, readFd, EVENT_ADD_NEW));
	event->setCgiFdRemoved(readFd, false);
	event->setIsStateChange(false);
}

void EventLoop::_changeToWriteSocket(Event *event)
{
	_removeAllCgiFds(event);
	sendMessage(Message(EVENTDEMUX_ID, event->getFd(), EVENT_CHANGE_TO_WRITE));
	event->setIsStateChange(false);
}

void EventLoop::_closeEvent(Event *event)
{
	int fd;

	fd = event->getFd();
	sendMessage(Message(EVENTDEMUX_ID, fd, EVENT_CHANGE_TO_READ));
	sendMessage(Message(CONNECTIONS_ID, fd, CONNECTION_RESTART_TIMER));
	_removeAllCgiFds(event);
	_deleteEvent(fd);
}

void EventLoop::_disconnectEvent(Event *event)
{
	int fd;

	fd = event->getFd();
	//if (!event->isFdRemoved())
	//{
		sendMessage(Message(EVENTDEMUX_ID, fd, EVENT_REMOVE));
		sendMessage(Message(CONNECTIONS_ID, fd, CONNECTION_REMOVE));
		//event->setfdRemoved();
	//}
	_removeAllCgiFds(event);
	_deleteEvent(fd);
}

void EventLoop::_insertEventToMap(std::map<int, Event*> &map, int fd, Event *event)
{
	if (event)
		map.insert(std::make_pair(fd, event));
}

void EventLoop::_removeCgiFd(Event *event, int cgiFd)
{
	if (cgiFd > 0 && !event->isCgiFdRemoved(cgiFd))
	{
		sendMessage(Message(EVENTDEMUX_ID, cgiFd, EVENT_REMOVE));
		event->setCgiFdRemoved(cgiFd, true);
	}
	_eventMap.erase(cgiFd);
}

void EventLoop::_removeAllCgiFds(Event *event)
{
	int	fd;

	fd = event->getFd();
	if (!event)
		return ;
	_removeCgiFd(event, event->getCgiWriteFd());
	_removeCgiFd(event, event->getCgiReadFd());
	_cgiEventMap.erase(fd);
}

void EventLoop::_deleteEvent(int fd)
{
	Event*	event;

	event = _getEventFromMap(_eventMap, fd);
	if (event)
		delete event;
	_eventMap.erase(fd);
}

// DEBUG
void EventLoop::_showEventMap(void)
{
	std::map<int, Event*>::iterator it;

	std::cout << "show _eventMap" << std::endl;
	for (it = _eventMap.begin(); it != _eventMap.end(); it++)
		std::cout << "fd: " << it->first << std::endl;
}

// DEBUG
void EventLoop::_showCgiEventMap(void)
{
	std::map<int, Event*>::iterator it;

	std::cout << "show _cgiEventMap" << std::endl;
	for (it = _cgiEventMap.begin(); it != _cgiEventMap.end(); it++)
		std::cout << it->first << std::endl;
}
