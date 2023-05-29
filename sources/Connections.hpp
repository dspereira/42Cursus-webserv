/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connections.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 11:51:36 by dsilveri          #+#    #+#             */
/*   Updated: 2023/05/29 17:33:01 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <sys/socket.h>
#include <sys/poll.h>
#include "Connection.hpp"
#include "Event.hpp"
//#include "Messenger.hpp"
#include "AMessengerClient.hpp"

class Connections: public AMessengerClient
{
	private:
		std::vector<Connection *>	_activeConnects;
		//Messenger					*_messenger;

		//void	_sendMessage(t_msg msg);

		void	_removeAllConnections(void);
		void	_removeConnection(int index);

	public:
		Connections(void);
		Connections(Messenger *messenger);
		~Connections(void);

		// new
		void			updateAllConnections(void);


		ClientID	getId(void);
		void		receiveMessage(Message *msg);

		// for debug only
		void			showConnections(void);
	
};
