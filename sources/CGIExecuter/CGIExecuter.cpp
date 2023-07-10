#include "CGIExecuter.hpp"

CGIExecuter::CGIExecuter(void)
{
	// Default CGIExecuter Constructor
}

CGIExecuter::~CGIExecuter(void)
{
	// Default CGIExecuter Destructor
}

void	CGIExecuter::execute(std::string script, std::string message)
{
	_scriptName = script;
	_scriptInterpreter = _getScriptInterpreter();

	if (_scriptInterpreter.empty())
		throw ExecutionErrorException();

	const char *av[] = {_scriptInterpreter.c_str(), _scriptName.c_str(), NULL};

	if (!_initPipes())
		throw FailToIinitPipesException();

	_pid = fork();
	if (_pid == -1)
		throw FailToCreateChildProcessException();

	if (_pid == 0)
	{
		close(_pipe1[1]);
		close(_pipe2[0]);

		dup2(_pipe1[0], STDIN_FILENO);
		dup2(_pipe2[1], STDOUT_FILENO);

		execve(_scriptInterpreter.c_str(), const_cast<char**>(av), NULL);

		throw ExecutionErrorException();
	}
	else
	{
		int i;
		i = write(_pipe1[1], message.c_str(), message.size());
		(void)i;

		close(_pipe1[0]);
		close(_pipe1[1]);
		close(_pipe2[1]);
	}
}

bool	CGIExecuter::isEnded(void)
{
	if (waitpid(_pid, NULL, WNOHANG) > 0)
		return (true);
	return (false);
}

int	CGIExecuter::getReadFD(void)
{
	return (_pipe2[0]);
}

std::string	CGIExecuter::getResult(void)
{
	char        buffer[BUFFER_SIZE];
    std::string res;
    size_t      nbrbytes;
	int			fd = _pipe2[0];

    while (1)
    {
        bzero(buffer, BUFFER_SIZE);
        nbrbytes = read(fd, buffer, BUFFER_SIZE - 1);
        if (nbrbytes < 1)
        	break;
        res += buffer;
    }
    close(fd);
    return (res);
}

/* Exceptions */

const char *CGIExecuter::ExecutionErrorException::what(void) const throw()
{
	return ("ExecutionErrorException: Error when try to execute an CGI Script.");
}

const char *CGIExecuter::FailToIinitPipesException::what(void) const throw()
{
	return ("FailToIinitPipesException: Error when try to open pipes.");
}

const char *CGIExecuter::FailToCreateChildProcessException::what(void) const throw()
{
	return ("FailToCreateChildProcessException: Error when try to create a child process.");
}

/* Class Private Functions */

bool	CGIExecuter::_initPipes(void)
{
	if (pipe(_pipe1) == -1 || pipe(_pipe2) == -1)
		return (false);
	return (true);
}

static void stringTrim(std::string &str);

std::string	CGIExecuter::_getScriptInterpreter(void)
{
	std::ifstream	file(_scriptName.c_str());
	std::string		result;
	std::string		line;

	if (file.is_open())
	{
		std::getline(file, line);
		stringTrim(line);
		result = line.substr(line.find_first_not_of("#!"));
	}
	return (result);
}

static void stringTrim(std::string &str)
{
	std::string	trimmed;
	size_t		start;
	size_t		end;
	size_t		len;

	start = str.find_first_not_of(WHITE_SPACE);
	end = str.find_last_not_of(WHITE_SPACE);
	len = end - start + 1;
	trimmed = str.substr(start, len);
	str = trimmed;
}
