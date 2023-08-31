#include "Configs.hpp"

static void			stringTrim(std::string &str);
static std::string	removeComments(std::string src);
static bool			isInsideQuotes(std::string src, size_t index);
static bool			isSingleColon(std::string &line);
static void			removeForwardWhiteSpaces(std::string &src);
static bool			isValidArray(std::string &array);
static bool			isValidNbrQuotes(std::string &data);

Configs::Configs(void)
{
	//Default Configs Constructor
}

Configs::Configs(const char *configsFileName)
{
	_initValidOptions();

	if (!_getConfigFile(configsFileName))
		throw InvalidConfigFileException();
	if (!_isValidConfigFile())
		throw InvalidConfigFileException();
	if (!_isValidConfigOptions())
		throw InvalidConfigFileException();
}

Configs::~Configs(void)
{
	//Default Configs Destructor
}

std::vector<std::string>	&Configs::getFileContentVector(void)
{
	return (_fileContentVector);
}

/* PRIVATE METHODS */

void	Configs::_initValidOptions(void)
{
	_validOptions.reserve(14);
	_validOptions.push_back("server");
	_validOptions.push_back("listen");
	_validOptions.push_back("error_pages");
	_validOptions.push_back("server_name");
	_validOptions.push_back("client_max_body_size");
	_validOptions.push_back("location");
	_validOptions.push_back("root");
	_validOptions.push_back("index");
	_validOptions.push_back("cgi");
	_validOptions.push_back("upload_store");
	_validOptions.push_back("redirect");
	_validOptions.push_back("allowed");
	_validOptions.push_back("master_root");
	_validOptions.push_back("mime_types");
	_validOptions.push_back("alias");
	_validOptions.push_back("autoindex");
}

bool	Configs::_getConfigFile(const char *configFile)
{
	std::ifstream	file(configFile);
	std::string		buff;

	if (file.is_open())
	{
		while (std::getline(file, buff))
		{
			buff = removeComments(buff);
			if (!buff.empty())
				_fileContentVector.push_back(buff);
		}
		file.close();
	}
	else
		return (false);
	return (true);
}

bool	Configs::_isValidConfigFile(void)
{
	for (size_t i = 0; i < _fileContentVector.size(); i++)
	{
		if (_fileContentVector.at(i).find_first_of('\t') != _fileContentVector.at(i).npos)
			return (false);
		if (!_isValidIndentationCount(_fileContentVector.at(i)))
			return (false);
		if (!_isOnlyServerWithoutIndentation(_fileContentVector.at(i)))
			return (false);
		if (!isValidNbrQuotes(_fileContentVector.at(i)))
			return (false);
		if (!isSingleColon(_fileContentVector.at(i)))
			return (false);
		if (!_isValidKey(_fileContentVector.at(i)))
			return (false);
		if (!_isValidValue(_fileContentVector.at(i)))
			return (false);
	}
	return (true);
}

bool	Configs::_isValidConfigOptions(void)
{
	std::vector<std::string>::iterator	it;
	std::string							key;
	bool								isValidOpt;

	it = _fileContentVector.begin();
	while (it != _fileContentVector.end())
	{
		isValidOpt = false;
		key = (*it).substr(0, (*it).find_first_of(":"));
		if (key.find_first_of("\"") == key.npos)
		{
			stringTrim(key);
			for (size_t i = 0; i < _validOptions.size(); i++)
			{
				if (key.compare(_validOptions.at(i)) == 0)
					isValidOpt = true;
			}
		}
		else
			isValidOpt = true;
		if (!isValidOpt)
			return (false);
		it++;
	}
	return (true);
}

bool	Configs::_isValidKey(std::string &line)
{
	std::string	key;

	key = line.substr(0, line.find_first_of(":"));
	stringTrim(key);
	if (key.find_first_not_of(VALID_KEY_LETTERS) != key.npos)
		return (false);
	return (true);
}

bool	Configs::_isValidValue(std::string &line)
{
	std::string	value;

	value = line.substr(line.find_first_of(":") + 1, line.size());
	stringTrim(value);
	if (value.find_first_of("[") != value.npos || value.find_first_of("]") != value.npos)
	{
		if (!isValidArray(value))
			return (false);
	}
	else if (value.find_first_of("\"") != value.npos)
	{
		if (!isValidNbrQuotes(value))
			return (false);
		if (value[0] != '\"' || value[value.size() - 1] != '\"')
			return (false);
	}
	return (true);
}

bool	Configs::_isValidIndentationCount(std::string &line)
{
	size_t	index;
	size_t	nbrSpaces;

	if (line.empty())
		return (false);
	nbrSpaces = 0;
	index = line.find_first_not_of(" ");
	for (size_t i = 0; i <= index; i++)
	{
		if (line[i] == ' ')
			nbrSpaces++;
	}
	if (nbrSpaces % 2 != 0 && nbrSpaces != 0)
		return (false);
	return (true);
}

bool	Configs::_isOnlyServerWithoutIndentation(std::string &line)
{
	if (line.compare("server:"))
	{
		size_t	i;
		i = line.find_first_not_of(" ");
		if (i == 0)
			return (false);
	}
	return (true);
}

/* UTILS FUNCTIONS */

static void	stringTrim(std::string &str)
{
	std::string	trimmed = str;
	size_t		start = trimmed.find_first_not_of(WHITE_SPACE);
	size_t		end = trimmed.find_last_not_of(WHITE_SPACE);

	if (start != std::string::npos)
		trimmed = trimmed.substr(start);
	if (end != std::string::npos)
		trimmed = trimmed.substr(0, end + 1);
	str = trimmed;
}

static std::string	removeComments(std::string src)
{
	std::string	result;

	result = src;
	for (size_t j = 0; j < result.size(); j++)
	{
		if (result[j] == '#')
		{
			if (!isInsideQuotes(result, j))
				result = result.substr(0, j);
		}
	}
	if (result.find_first_not_of(WHITE_SPACE) == result.npos)
		result.clear();
	removeForwardWhiteSpaces(result);
	return (result);
}

static bool	isInsideQuotes(std::string src, size_t index)
{
	int	amountQuotes = 0;

	for (size_t i = 0; i < index; i++)
	{
		if (src[i] == '\"')
			amountQuotes++;
	}
	if (amountQuotes % 2 == 0)
		return (false);
	return (true);
}

static bool	isSingleColon(std::string &line)
{
	int	count = 0;

	for (size_t i = 0; i < line.size(); i++)
		if (line[i] == ':' && !isInsideQuotes(line, i))
			count++;
	if (count != 1)
		return (false);
	return (true);
}

static void	removeForwardWhiteSpaces(std::string &src)
{
	size_t	i;

	i = src.find_last_not_of(WHITE_SPACE);
	if (i != src.npos)
		src = src.substr(0, i + 1);
}

static bool	isValidArray(std::string &array)
{
	size_t			nbrBrackets = 0;
	size_t			nbrQuotes = 0;
	std::string		temp;
	size_t			a = 1;

	for (size_t i = 0; i < array.size(); i++)
	{
		if (array[i] == '[')
			nbrBrackets++;
		else if (array[i] == ']')
			nbrBrackets--;
		else if (array[i] == '\"')
			nbrQuotes++;
	}
	if (nbrBrackets != 0 || nbrQuotes % 2 != 0)
		return (false);
	while (array[a])
	{
		while (array[a] != ',' && array[a + 1])
			temp += array[a++];
		if (temp.find_first_not_of(WHITE_SPACE) == temp.npos)
			return (false);
		temp.clear();
		a++;
	}
	return (true);
}

static bool	isValidNbrQuotes(std::string &data)
{
	size_t nbrQuotes = 0;

	for (size_t i = 0; i < data.size(); i++)
		if (data[i] == '\"')
			nbrQuotes++;
	if (nbrQuotes % 2 != 0)
		return (false);
	return (true);
}

/* EXCEPTIONS */

const char *Configs::InvalidConfigFileException::what(void) const throw()
{
	return ("Webserv: Invalid Config File");
}
