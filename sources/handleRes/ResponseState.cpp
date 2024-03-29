/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseState.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 16:15:08 by dsilveri          #+#    #+#             */
/*   Updated: 2023/09/25 11:51:44 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseState.hpp"

#include <sys/socket.h>
#include <iostream>
#include <EventType.hpp>

ResponseState::ResponseState(void) {}

ResponseState::~ResponseState(void) {}

StateResType ResponseState::handle(Event *event, ServerConfig& config)
{
	ssize_t				numBytesSend;
	ssize_t				resSize;
	const std::string	res = event->getRes();

	(void)config;
	resSize = res.size();
	numBytesSend = send(event->getFd(), res.c_str(), resSize, 0);
	if (numBytesSend <= 0)
	{
		event->setClientDisconnected();
		return (RESPONSE_END);
	}
	if (numBytesSend == resSize)
		event->clearRes();
	else
		event->eraseRes(0, numBytesSend);
	event->updateTotalBytesSend(numBytesSend);
	if (_isResponseFullySend(event->getTotalBytesSend(), event->getResSize()))
	{
		return (RESPONSE_END);
	}
	if (event->getFileSize())
		return (STATIC_FILE_HANDLING);
	return (RESPONSE);
}

bool ResponseState::_isResponseFullySend(size_t totalBytesSend, size_t resSize)
{
	if (totalBytesSend >= resSize)
		return (true);
	return (false);
}
