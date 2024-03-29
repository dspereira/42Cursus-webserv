/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 11:51:28 by dsilveri          #+#    #+#             */
/*   Updated: 2023/09/23 15:06:54 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/poll.h>

typedef enum 
{
    TIMER_PAUSED,
    TIMER_ACTIVE,
}	ConnectionStatus;

class Connection
{
	private:
		ConnectionStatus	_status;
		int					_fd;
		int					_keepAliveTimeout;
		time_t				_lastRequestTime;

	public:
		Connection(int fd);
		~Connection(void);

		int		getFd(void);
		bool	isKeepAliveTimeout(void);
		void	resetKeepAliveTimeout(void);
		void	startTimer(void);
		void	pauseTimer(void);
		void	showDataConnection(void);
};
