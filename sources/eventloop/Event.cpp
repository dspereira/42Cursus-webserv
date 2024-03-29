/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 11:15:31 by dsilveri          #+#    #+#             */
/*   Updated: 2023/09/27 14:23:50 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Event.hpp"
#include "Timer.hpp"
#include "EventType.hpp"
#include "configs.hpp"
#include <signal.h>
#include <unistd.h>

#define CONNECTION_CLOSED		1
#define CONNECTION_KEEPALIVE	0

Event::Event(int fd):
	_fd(fd),
	_reqState(HEADER_PROCESS),
	_totalBytesSend(0),
	_resState1(INITIAL_STATE),
	_creationTime(Timer::getActualTimeStamp()),
	_actualState(READ_SOCKET),
	_finished(false),
	_connectionClosed(-1),
	_clientDisconnect(false),
	_cgiExitStatus(0),
	_statusCode(0),
	_redirectCode(0),
	_fileSize(0),
	_fileNumBytesRead(0),
	_serverConf(NULL),
	_cgiWriteFd(-1),
	_cgiReadFd(-1),
	_cgiWriteFdClosed(false),
	_cgiReadFdClosed(false),
	_cgiPid(0),
	_numBytesSendCgi(0),
	_cgiScriptEndend(false),
	_isCgi(false),
	_fdRemoved(false),
	_cgiWriteFdRemoved(true),
	_cgiReadFdRemoved(true),
	_isStateChange(false),
	_isHttpsTested(false)
{
	SocketUtils::getHostAndPort(_fd, _ip, _port);
}

Event::~Event(void)
{
	if (_cgiPid > 0 && !_cgiScriptEndend)
	{
		if (kill(_cgiPid, SIGTERM) == -1)
			std::cout << "Webserv: Error terminating SGI script Event" << std::endl;
	}
	this->closeCgiWriteFd();
	this->closeCgiReadFd();
}

int Event::getFd(void)
{
	return (_fd);
}

StateReqType Event::getReqState(void)
{
	return (_reqState);
}

void Event::setReqState(StateReqType reqState)
{
	_reqState = reqState;
}

void Event::updateReqRawData(const std::string &req)
{
	_reqRaw.append(req, 0, req.size());
}

void Event::setReqRawData(const std::string &req)
{
	_reqRaw.assign(req, 0, req.size());
}

bool Event::isReqHeaderComplete(void)
{
	if (_reqRaw.find("\r\n\r\n") != _reqRaw.npos)
		return (true);
	return (false);
}

const std::string& Event::getReqHeader(void)
{
	size_t		idx;

	idx = _reqRaw.find("\r\n\r\n");
	if (idx == _reqRaw.npos)
		return (_reqHeader);
	_reqHeader = _reqRaw.substr(0, idx + 4);
	_reqRaw.erase(0, _reqHeader.size());
	return (_reqHeader);
}

void Event::parseReqHeader(std::string &header)
{
	this->setStatusCode(_reqParser.headerParse(header));
}

std::string Event::getReqRawData(void)
{
	return (_reqRaw);
}

void Event::clearReqRawData(void)
{
	_reqRaw.clear();
}

const std::string& Event::getRes(void)
{
	return (_res);
}

void Event::setRes(std::string& res)
{
	_res = res;
}

void Event::clearRes(void)
{
	_res.clear();
}

void Event::eraseRes(size_t start, size_t end)
{
	_res.erase(start, end);
}


void Event::updateRes(std::string& res)
{
	_res += res;
}

size_t Event::getResSize(void)
{
	return (_resSize);
}

void Event::setResSize(size_t resSize)
{
	_resSize = resSize;
}

size_t Event::getTotalBytesSend(void)
{
	return (_totalBytesSend);
}

void Event::updateTotalBytesSend(size_t totalBytesSend)
{
	_totalBytesSend += totalBytesSend;
}

StateResType Event::getResState1(void)
{
	return (_resState1);
}

void Event::setResState1(StateResType resState)
{
	_resState1 = resState;
}

bool Event::isEventTimeout(void)
{
	return (Timer::isTimeoutExpired(_creationTime, EVENT_TIMEOUT_SEC));
}

bool Event::isConnectionClose(void)
{
	if (_connectionClosed == -1)
	{
		if (!(_reqParser.getConnectionField().compare("close")))
			_connectionClosed = CONNECTION_CLOSED;
		else
			_connectionClosed = CONNECTION_KEEPALIVE;
	}
	if (_connectionClosed)
		return (true);
	return (false);
}

void Event::setConnectionClose(void)
{
	_connectionClosed = CONNECTION_CLOSED;
}

std::string		Event::getCgiScriptResult(void)
{
	return (_cgiScriptResult);
}

void	Event::updateCgiScriptResult(std::string& src)
{
	_cgiScriptResult += src;
}

std::string	Event::getQueryString(void)
{
	return (_reqParser.getQueryString());
}

std::string	Event::getReqContentType(void)
{
	std::vector<std::string> contentType;

	contentType = _reqParser.getHeaderField("content-type");
	if (!contentType.empty())
		return (contentType.at(0));
	return (std::string());
}

size_t	Event::getReqContentLength(void)
{
	std::vector<std::string>	contentLength;
	size_t						len;

	contentLength = _reqParser.getHeaderField("content-length");
	if (!contentLength.empty())
	{
		std::istringstream	ss(contentLength.at(0));
		if (!(ss >> len))
			return (0);
		return (len);
	}
	return (0);
}

std::string	Event::getReqTransferEncoding(void)
{
	std::vector<std::string> transferEncoding;

	transferEncoding = _reqParser.getHeaderField("transfer-encoding");
	if (!transferEncoding.empty())
		return (transferEncoding.at(0));
	return (std::string());
}

std::string Event::getReqHost(void)
{
	std::vector<std::string> host;

	host = _reqParser.getHeaderField("host");
	if (!host.empty())
		return (host.at(0));
	return (std::string());	
}

std::string	Event::getReqLineTarget(void)
{
	return (_reqParser.getReqLineTarget());
}

std::string	Event::getReqLineHttpVersion(void)
{
	return (_reqParser.getReqLineHttpVersion());
}

std::string	Event::getReqLineMethod(void)
{
	return (_reqParser.getReqLineMethod());
}

std::string	Event::getReqLinePath(void)
{
	return (_reqParser.getReqLinePath());
}

const std::string& Event::getReqBody(void)
{
	return (_reqBody);
}

EventType Event::getActualState(void)
{
	return (_actualState);
}

void Event::setActualState(EventType newState)
{
	_actualState = newState;
	this->setIsStateChange(true);
}

bool Event::isFinished(void)
{
	return (_finished);
}

void Event::setAsFinished(void)
{
	_finished = true;
}

bool Event::isClientDisconnect(void)
{
	return (_clientDisconnect);
}

void Event::setClientDisconnected(void)
{
	_clientDisconnect = true;
}

void Event::updateReqBody(const std::string& body)
{
	_reqBody.append(body, 0, body.size());
}

size_t Event::getReqBodySize(void)
{
	return (_reqBody.size());
}

int Event::getStatusCode(void)
{
	return (_statusCode);
}

void Event::setStatusCode(int statusCode)
{
	_statusCode = statusCode;
}

std::string Event::getIp(void)
{
	return (_ip);
}

std::string Event::getPort(void)
{
	return (_port);
}


void Event::setRredirectCode(int redirectCode)
{
	_redirectCode = redirectCode;
}

int Event::getRredirectCode(void)
{
	return (_redirectCode);
}

void Event::setRredirectResource(std::string redirectResource)
{
	_redirectResource = redirectResource;
}

std::string	Event::getRredirectResource(void)
{
	return (_redirectResource);
}

void Event::setFileSize(size_t fileSize)
{
	_fileSize = fileSize;
}

size_t Event::getFileSize(void)
{
	return (_fileSize);
}

void Event::updateFileNumBytesRead(size_t fileNumBytesRead)
{
	_fileNumBytesRead += fileNumBytesRead;
}

size_t Event::getFileNumBytesRead(void)
{
	return (_fileNumBytesRead);
}

void Event::setServerConfing(ServerConfig* serverConf)
{
	_serverConf = serverConf;
}

ServerConfig* Event::getServerConfing(void)
{
	return (_serverConf);
}

int Event::getCgiWriteFd(void)
{
	return (_cgiWriteFd);
}

void Event::setCgiWriteFd(int cgiWriteFd)
{
	_cgiWriteFd = cgiWriteFd;
}

int Event::getCgiReadFd(void)
{
	return (_cgiReadFd);
}

void Event::setCgiReadFd(int cgiReadFd)
{
	_cgiReadFd = cgiReadFd;
}

int Event::getCgiPid(void)
{
	return (_cgiPid);
}

void Event::setCgiPid(int pidCgi)
{
	_cgiPid = pidCgi;
}

ssize_t Event::getNumBytesSendCgi(void)
{
	return (_numBytesSendCgi);
}

void Event::updateNumBytesSendCgi(ssize_t numBytesSendCgi)
{
	_numBytesSendCgi += numBytesSendCgi;
}

void Event::closeCgiWriteFd(void)
{
	if (_cgiWriteFd < 0 || _cgiWriteFdClosed)
		return ;
	close(_cgiWriteFd);
	_cgiWriteFdClosed = true;
}

void Event::closeCgiReadFd(void)
{
	if (_cgiReadFd < 0 || _cgiReadFdClosed)
		return ;
	close(_cgiReadFd);
	_cgiReadFdClosed = true;
}

int Event::getCgiExitStatus(void)
{
	return (_cgiExitStatus);
}

void Event::setCgiExitStatus(int cgiExitStatus)
{
	_cgiExitStatus = cgiExitStatus;
}

bool Event::isCgiScriptEndend(void)
{
	return (_cgiScriptEndend);
}

void Event::setCgiScriptEndend(bool cgiScriptEndend)
{
	_cgiScriptEndend = cgiScriptEndend;
}

std::string Event::getRoute(void)
{
	return (_route);
}

void Event::setRoute(std::string route)
{
	_route = route;
}

std::string Event::getRequestPath(void)
{
	return (_requestPath);
}

void Event::setRequestPath(std::string requestPath)
{
	_requestPath = requestPath;
}

std::string Event::getResourcePath(void)
{
	return (_resourcePath);
}

void Event::setResourcePath(std::string resourcePath)
{
	_resourcePath = resourcePath;
}

void Event::setIsCgi(bool isCgi)
{
	_isCgi = isCgi;
}

bool Event::isCgi(void)
{
	return (_isCgi);
}

bool Event::isCgiWriteFdRemoved(void)
{
	return (_cgiWriteFdRemoved);
}

void Event::setCgiWriteFdRemoved(void)
{
	_cgiWriteFdRemoved = true;
}

bool Event::isCgiReadFdRemoved(void)
{
	return (_cgiReadFdRemoved);
}

void Event::setCgiReadFdRemoved(void)
{
	_cgiReadFdRemoved = true;
}

bool Event::isCgiFdRemoved(int cgiFd)
{
	if (cgiFd == _cgiReadFd)
		return (_cgiReadFdRemoved);
	if (cgiFd == _cgiWriteFd)
		return (_cgiWriteFdRemoved);
	return (false);
}

void Event::setCgiFdRemoved(int cgiFd, bool value)
{
	if (cgiFd == _cgiReadFd)
		_cgiReadFdRemoved = value;
	else if (cgiFd == _cgiWriteFd)
		_cgiWriteFdRemoved = value;
}

bool Event::isStateChange(void)
{
	return (_isStateChange);
}

void Event::setIsStateChange(bool isStateChange)
{
	_isStateChange = isStateChange;
}

bool Event::isHttpsTested(void)
{
	return (_isHttpsTested);
}

void Event::setHttpsTested(void)
{
	_isHttpsTested = true;
}