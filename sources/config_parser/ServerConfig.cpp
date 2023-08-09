#include "ServerConfig.hpp"

static size_t	getNumberOfIdentations(std::string &line);
static void		stringTrim(std::string &str);
static void		removeQuotes(std::string &str);
static bool		isInsideErrorPages(std::string &src);
static bool		isInsideLocations(std::string &src);
static bool		isNewLocations(std::string &src);
static bool		isInsideNewLocations(std::string &src);
static bool		isInsideMimeTypes(std::string &src);
static int		strToInt(std::string &str);

ServerConfig::ServerConfig(void)
{
	//Default ServerConfig Constructor
}

ServerConfig::ServerConfig(std::vector<std::string>	configs):
	_configError(true),
	_clientMaxBodySize(10000)
{
	std::vector<std::string>::iterator	it;
	std::string							key;
	std::string							value;

	it = configs.begin();
	while (it != configs.end() && _configError)
	{
		if (it != configs.end() && getNumberOfIdentations(*it) == 1)
		{
			key = _getKey(*it);
			if (key.compare("listen") == 0)
				_setListen(*it);
			else if (key.compare("server_name") == 0)
				_setServerName(*it);
			else if (key.compare("master_root") == 0)
				_setMasterRoot(*it);
			else if (key.compare("client_max_body_size") == 0)
				_setClientMaxBodySize(*it);
			else if (key.compare("error_pages") == 0)
				_setErrorPages(it, configs.end());
			else if (key.compare("location") == 0)
				_setLocations(it, configs.end());
			else if (key.compare("mime_types") == 0)
				_setMimeTypes(it, configs.end());
			else
				_updateConfigError(false);
		}
		if (!_isValidConfigError())
			break;
		it++;
	}
	if (_isValidConfigError())
		_checkAllLocationsStatus();
}

ServerConfig::~ServerConfig(void)
{
	//Default ServerConfig Destructor
}

bool	ServerConfig::getConfigError(void)
{
	return (_configError);
}

std::string	ServerConfig::getListen(void)
{
	return (_listen);
}

std::string	ServerConfig::getServerName(void)
{
	return (_serverName);
}

std::string ServerConfig::getMasterRoot(void)
{
	return(_masterRoot);
}

std::map<int, std::string>	ServerConfig::getErrorPages(void)
{
	return (_errorPages);
}

std::map<std::string, Location>&	ServerConfig::getLocations(void)
{
	return (_locations);
}

size_t	ServerConfig::getClientMaxBodySize(void)
{
	return (_clientMaxBodySize);
}

std::string	ServerConfig::getFilePathByRoute(std::string route)
{
	Location	*location;
	std::string	filePath;

	location = _getSpecificLocations(route);
	if (location)
	{
		filePath = location->getRoot();
		filePath += "/";
		filePath += location->getIndex();
	}
	return (filePath);
}

std::string	ServerConfig::getGlobalRoutePath(void)
{
	Location	*location;
	std::string	globalRoutePath;

	if (!_masterRoot.empty())
		return (_masterRoot);
	location = _getSpecificLocations("/");
	if (location)
		globalRoutePath = location->getRoot();
	return (globalRoutePath);
}

std::string	ServerConfig::existMimeType(std::string src)
{
	std::map<std::string, std::string>::iterator	it;
	std::string	res;
	std::string	srcWithoutDot;
	size_t	i;

	i = src.find_first_of(".");
	if (i != src.npos)
		srcWithoutDot = src.substr(i + 1);
	
	it = _mimeTypes.find(src);
	if (it != _mimeTypes.end())
		res = it->second;
	else
	{
		it = _mimeTypes.find(srcWithoutDot);
		if (it != _mimeTypes.end())
			res = it->second;
	}
	return (res);
}

/* PRIVATE METHODS */

void	ServerConfig::_updateConfigError(bool newConfigError)
{
	_configError = newConfigError;
}

bool	ServerConfig::_isValidConfigError(void)
{
	if (_configError == false)
		return (false);
	return (true);
}

std::string	ServerConfig::_getValue(std::string &src)
{
	std::string value;
	size_t		index;

	index = src.find_first_of(":");
	value = src.substr(index + 1);
	stringTrim(value);
	removeQuotes(value);
	return (value);
}

std::string	ServerConfig::_getKey(std::string &src)
{
	std::string	key;
	size_t		begin;
	size_t		end;

	begin = src.find_first_not_of(EMPTY_SPACE);
	end = src.find_first_of(":");
	key = src.substr(begin, end - begin);
	stringTrim(key);
	removeQuotes(key);
	return (key);
}

void	ServerConfig::_setListen(std::string newListen)
{
	_listen = _getValue(newListen);
	if (_listen.empty())
		_updateConfigError(false);
}

void	ServerConfig::_setServerName(std::string newServerName)
{
	_serverName = _getValue(newServerName);
	if (_serverName.empty())
		_updateConfigError(false);
}

void	ServerConfig::_setMasterRoot(std::string newMasterRoot)
{
	_masterRoot = _getValue(newMasterRoot);
	if (_masterRoot.empty())
		_updateConfigError(false);
}

void	ServerConfig::_setErrorPages(std::vector<std::string>::iterator &it,
	std::vector<std::string>::iterator itEnd)
{
	std::string temp;

	temp = _getValue(*it);
	if (!temp.empty())
	{
		_updateConfigError(false);
		return ;
	}
	it++;
	if (!isInsideErrorPages(*it))
		_updateConfigError(false);
	else
	{
		while (it != itEnd && isInsideErrorPages(*it))
		{
			_addNewErrorPage(*it);
			it++;
		}
		it--;
	}
}

void	ServerConfig::_addNewErrorPage(std::string &src)
{
	std::string key;
	int			intKey;
	std::string value;

	key = _getKey(src);
	value = _getValue(src);
	if (key.find_first_not_of("0123456789") == key.npos)
	{
		intKey = strToInt(key);
		_errorPages.insert(std::make_pair(intKey, value));
	}
	else
		_updateConfigError(false);
}

void	ServerConfig::_setClientMaxBodySize(std::string &value)
{
	std::stringstream	out;
	std::string strValue;
	size_t	size;

	strValue = _getValue(value);
	if (strValue.find_first_not_of("0123456789") != strValue.npos)
		_updateConfigError(false);
	else
	{
		out << strValue;
		out >> size;
		_clientMaxBodySize = size;
	}
}

void	ServerConfig::_setLocations(std::vector<std::string>::iterator &it,
	std::vector<std::string>::iterator itEnd)
{
	std::string					newLocationURL;
	std::vector<std::string>	locationInfo;
	std::string temp;

	temp = _getValue(*it);
	if (!temp.empty())
	{
		_updateConfigError(false);
		return ;
	}
	it++;
	if (!isInsideLocations(*it))
		_updateConfigError(false);
	else
	{
		while (it != itEnd && isInsideLocations(*it))
		{
			if (it != itEnd && isNewLocations(*it))
			{
				newLocationURL = _getKey(*it);
				it++;
				while (it != itEnd && isInsideNewLocations(*it))
				{
					locationInfo.push_back(*it);
					it++;
				}
				_locations.insert(std::make_pair(newLocationURL, Location(_masterRoot, locationInfo)));
				locationInfo.clear();
				it--;
			}
			else
			{
				_updateConfigError(false);
				break;
			}
			it++;
		}
		it--;
	}
}

void	ServerConfig::_setMimeTypes(std::vector<std::string>::iterator &it,
	std::vector<std::string>::iterator itEnd)
{
	std::string temp;

	temp = _getValue(*it);
	if (!temp.empty())
	{
		_updateConfigError(false);
		return ;
	}
	it++;
	if (!isInsideMimeTypes(*it))
		_updateConfigError(false);
	else
	{
		while (it != itEnd && isInsideMimeTypes(*it))
		{
			_addNewMimeType(*it);
			it++;
		}
		it--;
	}
}

void	ServerConfig::_addNewMimeType(std::string &src)
{
	std::string key;
	std::string value;

	key = _getKey(src);
	value = _getValue(src);
	_mimeTypes.insert(std::make_pair(key, value));
}

void	ServerConfig::_checkAllLocationsStatus(void)
{
	std::map<std::string, Location>::iterator	it;

	for (it = _locations.begin(); it != _locations.end(); it++)
	{
		if (it->second.getLocationError() == false)
		{
			_updateConfigError(false);
			break;
		}
	}
}

Location	*ServerConfig::_getSpecificLocations(std::string location)
{
	std::map<std::string, Location>::iterator	it;

	it = _locations.find(location);
	if (it != _locations.end())
		return (&(it->second));
	return (NULL);
}

/* STATIC FUNCTIONS */

static bool	isInsideErrorPages(std::string &src)
{
	if (getNumberOfIdentations(src) == 2)
		return (true);
	return (false);
}

static bool	isInsideLocations(std::string &src)
{
	if (getNumberOfIdentations(src) >= 2)
		return (true);
	return (false);
}

static bool	isNewLocations(std::string &src)
{
	if (getNumberOfIdentations(src) == 2)
		return (true);
	return (false);
}

static bool	isInsideNewLocations(std::string &src)
{
	if (getNumberOfIdentations(src) > 2)
		return (true);
	return (false);
}

static bool	isInsideMimeTypes(std::string &src)
{
	if (getNumberOfIdentations(src) == 2)
		return (true);
	return (false);
}

static size_t	getNumberOfIdentations(std::string &line)
{
	size_t	index;
	size_t	identations = 0;
	size_t	nbrSpaces = 0;

	if (line.empty())
		return (0);
	index = line.find_first_not_of(" ");
	for (size_t i = 0; i <= index; i++)
	{
		if (nbrSpaces == 2)
		{
			identations++;
			nbrSpaces = 0;
		}
		if (line[i] == ' ')
			nbrSpaces++;
	}
	return (identations);
}

static void	stringTrim(std::string &str)
{
	std::string	trimmed = str;
	size_t		start = trimmed.find_first_not_of(EMPTY_SPACE);
	size_t		end = trimmed.find_last_not_of(EMPTY_SPACE);

	if (start != std::string::npos)
		trimmed = trimmed.substr(start);
	if (end != std::string::npos)
		trimmed = trimmed.substr(0, end + 1);
	str = trimmed;
}

static void	removeQuotes(std::string &str)
{
	size_t	begin, end;

	begin = str.find_first_of("\"");
	if (begin != str.npos)
	{
		end = str.find_last_of("\"");
		if (end != str.npos)
			str = str.substr(begin + 1, end - 1);
	}
}

static int	strToInt(std::string &str)
{
	std::stringstream	out(str);
	int					intValue;
	out >> intValue;
	return (intValue);
}