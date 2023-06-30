/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 14:54:59 by dsilveri          #+#    #+#             */
/*   Updated: 2023/06/29 16:38:56 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "IEventHandler.hpp"
#include "RequestParser.hpp"
#include "RequestData.hpp"
#include "HandleReq.hpp"


#include "Configs.hpp"
#include "ConfigsData.hpp"

class ReadHandler: public IEventHandler
{
	private:
		//ConfigsData	_data;
		HandleReq	*_handleReq;

	public:
		ReadHandler(void);
		ReadHandler(HandleReq *handleReq);
		//ReadHandler(ConfigsData	data);
		~ReadHandler(void);
		ReadHandler(const ReadHandler &src);
		ReadHandler &operator=(const ReadHandler &src);

		void		handleEvent(Event *event);
		EventType	getHandleType(void);
};