/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messenger.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/20 17:11:39 by dsilveri          #+#    #+#             */
/*   Updated: 2023/05/30 17:49:24 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Messenger.hpp"

#include <iostream>

Messenger::Messenger(void)
{
	//Default Messenger Constructor
}

Messenger::Messenger(const Messenger &src)
{
	//Messenger Copy Constructor
}

Messenger::~Messenger(void)
{
	//Default Messenger Destructor
}

/*
Messenger &Messenger::operator=(const Messenger &src)
{
	//Messenger Copy Assignment Operator
}
*/

void Messenger::registerClient(AMessengerClient *client)
{
	ClientID id;

	id = client->getId();
	_clients.insert(std::pair<ClientID, AMessengerClient*>(id, client));
}

void Messenger::unregisterClient(ClientID clientID)
{
	_clients.erase(clientID);
}

void Messenger::sendMessage(Message *msg)
{
	std::map<ClientID, AMessengerClient*>::iterator it;

	if (_clients.size())
	{
		it = _clients.find((ClientID) msg->getDst());
		it->second->receiveMessage(msg);
	}
	delete msg;
}