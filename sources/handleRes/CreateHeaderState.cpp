/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CreateHeaderState.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 11:43:02 by dsilveri          #+#    #+#             */
/*   Updated: 2023/07/04 16:41:40 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CreateHeaderState.hpp"

#include "ErrorPageBuilder.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

CreateHeaderState::CreateHeaderState(void)
{
	//Default CreateHeaderState Constructor
}

CreateHeaderState::CreateHeaderState(const CreateHeaderState &src)
{
	//CreateHeaderState Copy Constructor
}

CreateHeaderState::~CreateHeaderState(void)
{
	//Default CreateHeaderState Destructor
}

/*
CreateHeaderState &CreateHeaderState::operator=(const CreateHeaderState &src)
{
	//CreateHeaderState Copy Assignment Operator
}
*/

/*
StateResType CreateHeaderState::handle(Event *event, ConfigsData configsData)
{
	std::string fileName;
	std::string	header;
	size_t		fileSize;

	//std::cout << "CreateHeaderState" << std::endl;

	fileName = _getFileName(event->getReqPath(), configsData);
	_isFileReadable(fileName);
	fileSize = _getFileSize(fileName);
	event->setBodySize1(fileSize);
	_createHeader(header, fileName);
	event->setFileName(fileName);
	event->setRes(header);
	event->setResSize(header.size() + fileSize);
	return (GET_BODY);
}
*/


// Tem de ser reorganizado de outra forma
StateResType CreateHeaderState::handle(Event *event, ConfigsData configsData)
{
	std::string			fileName;
	std::string			header;
	size_t				fileSize;
	int					errorCode = 0;
	ErrorPageBuilder	errorBuilder;


	fileName = _getFileName(event->getReqPath(), configsData);
	if (_isFileReadable(fileName))
		fileSize = _getFileSize(fileName);
	else
	{
		errorCode = 404;
		event->setErrorCode(errorCode);
		errorBuilder.setErrorCode(errorCode);
		fileSize = errorBuilder.getErrorPageSize();
	}
	event->setBodySize1(fileSize);

	if (errorCode)
		_createHeaderDefaultError(header, errorCode);
	else
		_createHeader(header, fileName);
	
	event->setFileName(fileName);
	event->setRes(header);
	event->setResSize(header.size() + fileSize);
	return (GET_BODY);
}

std::string CreateHeaderState::_getFileName(std::string reqTarget, ConfigsData conf)
{
	std::string fileName;
	std::string path;

	if (!reqTarget.compare("/"))
		fileName = conf.getConfig("root");
	else
		fileName = conf.getConfig(reqTarget);

	// Path configuration is expected to be a valid file path pointing to the location of the pages.
	path = conf.getConfig("path");
	if (fileName.empty() && !path.empty())
		fileName = path + "/" + reqTarget;
	return (fileName);
}

bool CreateHeaderState::_isFileReadable(std::string fileName)
{
	std::ifstream file(fileName.c_str());

	if (!file.good())
		return (false);
	file.close();
	return (true);
}

size_t CreateHeaderState::_getFileSize(std::string fileName)
{
	size_t size;
	std::ifstream file(fileName.c_str());

	size = 0;
	if (!file.good())
		return (size);
    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.close();	
	return (size);
}

std::string CreateHeaderState::_getFileType(std::string fileName)
{
	std::string type;
	size_t		dotIdx;

	dotIdx = (fileName.find_last_of('.')) + 1;
	if (dotIdx < fileName.size())
		type = fileName.substr(dotIdx, fileName.size());
	return (type);
}

// criar ficheiro default com mime types
std::string CreateHeaderState::_getMimeType(std::string fileExt)
{
	std::string mimeType;

	if (!fileExt.compare("html"))
		mimeType = "text/html";
	else if (!fileExt.compare("html"))
		mimeType = "text/css";
	else if (!fileExt.compare("js"))
		mimeType = "text/javascript";
	else if (!fileExt.compare("png"))
		mimeType = "image/x-png";
	else if (!fileExt.compare("jpg"))
		mimeType = "image/jpeg";
	else if (!fileExt.compare("iso")) // just for test
		mimeType = "text/plain";
	return (mimeType);
}
 
void CreateHeaderState::_createHeader(std::string &header, std::string fileName)
{
	std::stringstream	bodySize;

	bodySize << _getFileSize(fileName);
	header = "HTTP/1.1 200 OK\r\nContent-length: ";
	header += bodySize.str();
	header += "\r\n";
	header += "Content-Type: " + _getMimeType(_getFileType(fileName));
	header += "\r\n\r\n";
}

void CreateHeaderState::_createHeaderDefaultError(std::string &header, int errorCode)
{
	ErrorPageBuilder	errorBuilder(errorCode);
	std::stringstream	bodySize;

	std::cout << "phrase: " << errorBuilder.getReasonPhrase() << std::endl;

	bodySize << errorBuilder.getErrorPageSize();
	header = "HTTP/1.1 ";
	header += errorBuilder.getErrorCodeToString() + " "; 
	header += errorBuilder.getReasonPhrase();
	header += "\r\nContent-length: ";
	header += bodySize.str();
	header += "\r\n";
	header += "Content-Type: text/html";
	header += "\r\n\r\n";
}
