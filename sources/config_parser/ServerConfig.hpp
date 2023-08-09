#pragma once

#include "Location.hpp"

class ServerConfig
{
	private:

		bool		_configError;

		std::string	_serverName;
		std::string	_listen;
		std::string	_masterRoot;
		size_t		_clientMaxBodySize;

		std::map<std::string, Location>			_locations;
		std::map<int, std::string>				_errorPages;
		std::map<std::string, std::string>		_mimeTypes;

		void		_updateConfigError(bool newConfigError);
		bool		_isValidConfigError(void);
		void		_setListen(std::string newListen);
		void		_setServerName(std::string newServerName);
		void		_setMasterRoot(std::string newMasterRoot);
		void		_setClientMaxBodySize(std::string &value);
		std::string	_getKey(std::string &src);
		std::string	_getValue(std::string &src);
		void		_setErrorPages(std::vector<std::string>::iterator &it,
			std::vector<std::string>::iterator itEnd);
		void		_setLocations(std::vector<std::string>::iterator &it,
			std::vector<std::string>::iterator itEnd);
		void		_setMimeTypes(std::vector<std::string>::iterator &it,
			std::vector<std::string>::iterator itEnd);
		void		_addNewErrorPage(std::string &src);
		void		_addNewMimeType(std::string &src);
		void		_checkAllLocationsStatus(void);
		Location*	_getSpecificLocations(std::string location);

	public:
		ServerConfig(void);
		~ServerConfig(void);
		ServerConfig(std::vector<std::string> configs);

		bool								getConfigError(void);
		std::string							getListen(void);
		std::string							getServerName(void);
		std::string							getMasterRoot(void);
		std::map<int, std::string>			getErrorPages(void);
		std::map<std::string, Location>&	getLocations(void);
		size_t								getClientMaxBodySize(void);
		std::string							getFilePathByRoute(std::string route);
		std::string							getGlobalRoutePath(void);
		std::string							existMimeType(std::string src);
};