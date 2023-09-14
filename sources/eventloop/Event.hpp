/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsilveri <dsilveri@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 11:15:26 by dsilveri          #+#    #+#             */
/*   Updated: 2023/09/14 11:56:20 by dsilveri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include "RequestData.hpp"
#include "RequestParser.hpp"
#include "Configs.hpp"
#include "ConfigsData.hpp"
#include "StateReqType.hpp"
#include "StateResType.hpp"

#include "EventType.hpp"
#include "SocketUtils.hpp"


//#define	NONE			0
#define HEADER_HANDLE	0
#define	HEADER_DONE		1
#define	BODY_HANDLE		2
#define	BODY_DONE		3

#define	NO_EXIT_STATUS	256


class Event
{
	private:
		std::string		_reqRaw;
		RequestData		_reqParsed;
		RequestParser 	_reqParser;
		std::string		_res;

		std::string	_headerRaw;
		std::string _bodyRaw;

		int			_fd;
		short		_state;
		short		_parseState;

		StateReqType	_reqState;

		int			_resState;

		std::vector<std::string> _resVect;
		ssize_t _numWrited;
		int _idx;

		// for andleRes
		std::string		_fileName;
		size_t			_bodySize;   // talvez não seja necessário devido ao _resSize
		size_t			_bytesReadBody;
		size_t			_resSize;
		size_t			_totalBytesSend;
		StateResType	_resState1;
		int				_errorCode;

		//CGI
		bool			_cgiFlag;


		std::string		_cgiScriptResult;

		//Timeout: time for handle all request and all response 
		int 	_timeoutSec;
		time_t	_creationTime;

		bool	_clientClosed;


		EventType _oldState;
		EventType _actualState;

		bool	_finished;

		short	_connectionClosed;
		bool	_clientDisconnect;
		int		_cgiExitStatus;
		size_t	_cgiSentChars;

		std::string _body;

		std::string _reqHeader;

		int _statusCode;

		std::string _ip;
		std::string _port;
		std::string	_cgiBodyRes;

		//std::string	_resourcePath;

		int			_redirectCode;
		std::string	_redirectResource;

		size_t	_fileSize;
		size_t	_fileNumBytesRead;

		ServerConfig*	_serverConf;


		/*int _fdCgiW;
		int _fdCgiR;
		int _pidCgi;*/

		int	_cgiWriteFd;
		int	_cgiReadFd;

		bool	_cgiWriteFdRemoved;
		bool	_cgiReadFdRemoved;

		bool	_cgiWriteFdClosed;
		bool	_cgiReadFdClosed;

		int _cgiPid;

		//bool	_autoindex;

		ssize_t _numBytesSendCgi;

		bool	_cgiScriptEndend;
		//int		_cgiExitStatus;

		std::string _route;
		std::string _requestPath;
		std::string _resourcePath;

		bool _isCgi;

	public:
		Event(void);
		Event(int fd, int state);
		Event(const Event &src);
		~Event(void);
		Event &operator=(const Event &src);

		int			getFd(void);
		short		getState(void);
		std::string&	getResponse(void);
		std::string getReqRaw(void);
		std::string getHeaderRaw(void);
		short		getParseState(void);
		size_t		getBodySize(void);


		void		setState(short state);
		void		setResponse(std::string	res);

		void		setResquestHeader(std::string reqLine, std::map<std::string, std::vector<std::string> > reqHeader);
		void		setResquestBody(std::string body);

		void		setParseState(int state);

		//void		updateReqRaw(std::string req);

		bool		isBodyComplete(void);

		// void		createResponse(ConfigsData &configsData);


		//New
		StateReqType			getReqState(void);
		void				setReqState(StateReqType reqState);
		void				updateReqRawData(std::string &req);
		const std::string&  getReqRaw1(void);

		void setHeaderRaw(std::string header);
		void setBodyRaw(std::string body);
		std::string getHeaderRaw1(void);
		std::string getBodyRaw(void);
		void setReqRaw1(std::string req);


		void setResState(int resState);
		int getResState(void);

		void setResVect(void);

		void printVectDebug(void);

		std::string& getNextRes(void);

		void updateRes1(std::string res);

		void updateIdx(void);

		bool lastIdx(void);

		ssize_t getNumWrited(void);
		void updateNumWrited(ssize_t numWrited);

		std::string getReqPath(void);


		// for andleRes
		std::string	getFileName(void);
		void		setFileName(std::string fileName);

		size_t		getBytesReadBody(void);
		void		setBytesReadBody(size_t bytesReadBody);
		void		updateBytesReadBody(size_t bytesReadBody);

		size_t		getBodySize1(void);
		void		setBodySize1(size_t bodySize);

		const std::string&	getRes(void);
		void				setRes(std::string& res);
		void 				clearRes(void);
		void				eraseRes(size_t start, size_t end);
		void				updateRes(std::string& res);

		size_t				getResSize(void);
		void				setResSize(size_t resSize);

		size_t				getTotalBytesSend(void);
		void				setTotalBytesSend(size_t totalBytesSend);
		void				updateTotalBytesSend(size_t totalBytesSend);

		StateResType		getResState1(void);
		void				setResState1(StateResType resState);

		int					getErrorCode(void);
		void				setErrorCode(int errorCode);

		//CGI functions
		bool			getCgiFlag(void);
		void			setCgiFlag(bool cgiFlag);


		std::string		getCgiScriptResult(void);
		void			updateCgiScriptResult(std::string& src);

		bool				isEventTimeout(void);

		bool				isConnectionClose(void);
		void				setConnectionClose(void);


		bool				isClientClosed(void);
		void				setClientClosed(void);

		void				updateCgiSentChars(size_t value);
		size_t				getCgiSentChars(void);

		std::string			getCgiBodyRes(void);
		void				setCgiBodyRes(std::string &src);

		// Important Criar uma função para eliminar o CGI (Fazer delete) Verificar se já é feito
		int					getCgiFd(void);

		int					getCgiWriteFd(void);
		int					getCgiReadFd(void);

		int					isCgiScriptEnd(void);
		std::string					getQueryString(void);
		std::vector<std::string>	getRequestHeaderValue(std::string key);
		std::string					getReqMethod(void);
		std::string					getServerProtocol(void);
		std::string					getReqContentType(void);
		size_t						getReqContentLength(void);
		

		
		//New functions of request parser
		std::string		getReqLineTarget(void);
		std::string		getReqLineHttpVersion(void);
		std::string		getReqLineMethod(void);
		std::string		getReqLinePath(void);
		void			setReqBody(std::string body);
		std::string&	getReqBody(void);
		void			parseHeader(std::string &header);

		std::string		getReqTransferEncoding(void);
		std::string		getReqHost(void);


		EventType	getOldState(void);
		EventType	getActualState(void);
		void		setActualState(EventType actualState);

		bool	isFinished(void);
		void	setAsFinished(void);

		bool	isClientDisconnect(void);
		void	setClientDisconnected(void);


		void	cgiExecute(ServerConfig *config, std::string scriptName);
		int		writeToCgi(const char *str, size_t size);
		int		readFromCgi(std::string &str);

		//void	setCgiExitStatus(int cgiExitStatus);
		//int		getCgiExitStatus(void);

		std::string	getBody(void);		
		void		updateReqBody(std::string body);
		size_t		getReqBodySize(void);


		bool isReqHeaderComplete(void);
		void parseReqHeader(std::string &header);
		const std::string& getReqHeader(void);
		const std::string& getReqRawData(void);
		void clearReqRawData(void);

		void	setBody(std::string &src);

		int getStatusCode(void);
		void setStatusCode(int statusCode);

		std::string getIp(void);
		std::string getPort(void);

		//void setResourcePath(std::string resourcePath);
		//std::string getResourcePath(void);

		int _redirectionCode;
		std::string _redirectionResource;

		void		setRredirectCode(int redirectCode);
		int			getRredirectCode(void);
		void		setRredirectResource(std::string redirectResource);
		std::string	getRredirectResource(void);

		void setFileSize(size_t fileSize);
		size_t getFileSize(void);
		void setFileNumBytesRead(size_t fileNumBytesRead);
		void updateFileNumBytesRead(size_t fileNumBytesRead);
		size_t getFileNumBytesRead(void);

		void			setServerConfing(ServerConfig* serverConf);
		ServerConfig*	getServerConfing(void);

		//void setAutoindex(bool autoindex);
		//bool isAutoindex(void);


		/*
		void setFdCgiW(int fdCgiW);
		int getFdCgiW(void);
		void setFdCgiR(int fdCgiR);
		int getFdCgiR(void);
		void setPidCgi(int pidCgi);
		int getPidCgi(void);
		*/

		void setCgiWriteFd(int cgiWriteFd);
		void setCgiReadFd(int cgiReadFd);

		int getCgiPid(void);
		void setCgiPid(int pidCgi);

		ssize_t getNumBytesSendCgi(void);
		void setNumBytesSendCgi(ssize_t numBytesSendCgi);
		void updateNumBytesSendCgi(ssize_t numBytesSendCgi);

		void closeCgiWriteFd(void);
		void closeCgiReadFd(void);

		int getCgiExitStatus(void);
		void setCgiExitStatus(int cgiExitStatus);

		bool isCgiScriptEndend(void);
		void setCgiScriptEndend(bool cgiScriptEndend);

		std::string getRoute(void);
		void setRoute(std::string route);

		std::string getRequestPath(void);
		void setRequestPath(std::string requestPath);

		std::string getResourcePath(void);
		void setResourcePath(std::string resourcePath);


		//void setResourcePath(std::string resourcePath);
		//std::string getResourcePath(void);


	/*
		bool	_cgiWriteFdRemoved;
		bool	_cgiReadFdRemoved;

		bool	_cgiWriteFdClosed;
		bool	_cgiReadFdClosed;
	*/

	//bool _isCgiFdRemoved(int cgiFd);
	//bool _isCgiFdClosed(int cgiFd);

	//void _markCgiFdAsRemoved(int cgiFd);
	//void _markCgiFdAsClosed(int cgiFd);


	
	void setIsCgi(bool isCgi);
	bool isCgi(void);


};
