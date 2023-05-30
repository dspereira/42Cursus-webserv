#include "RequestParserUtils.hpp"

static std::vector<std::string>								getElementValue(const std::string &src);
static std::pair<std::string, std::vector<std::string> >	getPair(std::string &src);
static std::string											stringTrim(const std::string &str);
static std::string											removeQuotes(const std::string &src);

std::string	RequestParserUtils::getDataString(int fd)
{
	char		buff[BUFFER_SIZE + 1];
	std::string	data;
	int			bytesRead;

	while (true)
	{
		std::memset(buff, 0, BUFFER_SIZE + 1);

		bytesRead = read(fd, buff, BUFFER_SIZE);
		if (bytesRead <= 0)
			break;
		data += buff;
	}
	return (data);
}

std::string	RequestParserUtils::getRequestLine(std::vector<std::string> &src)
{
	std::string	line;

	line = src.begin()->c_str();
	line = stringTrim(line);
	return (line);
}

std::vector<std::string>	RequestParserUtils::getDataVector(std::string &src)
{
	std::vector<std::string>	result;
	std::istringstream			iss(src);
	std::string					line;

	while (std::getline(iss, line))
		result.push_back(line + "\n");
	return (result);
}

std::map<std::string, std::vector<std::string> >	RequestParserUtils::getRequestHeader(std::vector<std::string> &src)
{
	std::map<std::string, std::vector<std::string> >	header;
	std::string											temp;

	for (size_t i = 1; i < src.size(); i++)
	{
		temp = src.at(i).c_str();
		if (temp.find_first_not_of("\r\n") == temp.npos)
			break;
		header.insert(getPair(temp));
	}
	return (header);
}

std::string	RequestParserUtils::getBody(std::vector<std::string> &src)
{
	std::string	body;
	bool		start_body = false;

	body.clear();
	for (size_t i = 1; i < src.size(); i++)
	{
		if (start_body)
			body += src.at(i).c_str();
		if (src.at(i).find_first_not_of("\r\n") == src.at(i).npos)
			start_body = true;
	}
	return (body);
}

static std::pair<std::string, std::vector<std::string> >	getPair(std::string &src)
{
	std::string					key;
	std::vector<std::string>	value;
	std::string					temp;
	size_t		index;

	key.clear();
	value.clear();
	index = src.find_first_of(":");
	if (index != src.npos)
	{
		if (src.find_last_not_of(WHITE_SPACE, index - 1) != src.npos)
		{
			key = src.substr(0, index);
			for (size_t i = 0; i < key.size(); i++)
				key[i] = std::tolower(key[i]);
		}
		if (src.find_first_not_of(WHITE_SPACE, index + 1) != src.npos)
		{
			temp = src.substr(src.find_first_of(":") + 1, src.size());
			temp = stringTrim(temp);
			value = getElementValue(temp);
		}
	}
	return (std::make_pair(key, value));
}

static std::string	stringTrim(const std::string &str)
{
	std::string	trimmed;
	size_t		start;
	size_t		end;
	size_t		len;

	start = str.find_first_not_of(WHITE_SPACE);
	end = str.find_last_not_of(WHITE_SPACE);
	len = end - start + 1;
	trimmed = str.substr(start, len);
	return (trimmed);
}

static bool	isValidNumberOfQuotes(const std::string &src);

static std::vector<std::string>	getElementValue(const std::string &src)
{
	std::vector<std::string>	elements;
	std::string					temp;
	size_t						nbrQuotes = 0;
	size_t						j = 0;

	elements.clear();
	if (isValidNumberOfQuotes(src))
	{
		for (size_t i = 0; i <= src.size(); i++)
		{
			if (src[i] == '\"')
				nbrQuotes++;
			if (i == src.size() || (src[i] == ',' && nbrQuotes % 2 == 0))
			{
				temp = src.substr(j, i - j);
				temp = removeWhiteSpacesAndQuotes();
				temp = removeQuotes(temp);
				elements.push_back(temp);
				temp.clear();
				nbrQuotes = 0;
				j = i + 1;
			}
		}
	}
	return (elements);
}

static bool	isValidNumberOfQuotes(const std::string &src)
{
	size_t	nbrQuotes = 0;

	for (size_t i = 0; i < src.size(); i++)
	{
		if (src[i] == '\"')
			nbrQuotes++;
	}
	if (nbrQuotes % 2 != 0)
		return (false);
	return (true);
}

static std::string	removeQuotes(const std::string &src)
{
	std::string	res;
	size_t		index_1;
	size_t		index_2;

	res = src;
	index_1 = res.find_first_not_of("\"");
	index_2 = res.find_last_not_of("\"");
	if (index_1 == res.npos || index_2 == res.npos)
		return (res);
	res = res.substr(index_1, index_2 - index_1 + 1);
	return (res);
}
