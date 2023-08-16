#pragma once

#define SERVER_SOFTWARE			"webserv/1.0"

#define SOCKET_READ_BUFF_SIZE	100000
#define DEFAULT_PORT_STR		"80"
#define	MAX_PORT_VALUE			65535
#define	MIN_PORT_VALUE			0
#define DEFAULT_HOST			"localhost"

// socket defines
#define DEFAULT_BACKLOG 		128

#define EPOLL_MAX_NEVENTS	100000
#define EPOLL_TIMEOUT		5

// Colors
#define RESET		"\033[0m"
#define BLACK		"\033[30m"      /* Black */
#define RED			"\033[31m"      /* Red */
#define GREEN		"\033[32m"      /* Green */
#define YELLOW		"\033[33m"      /* Yellow */
#define BLUE		"\033[34m"      /* Blue */
#define MAGENTA		"\033[35m"      /* Magenta */
#define CYAN		"\033[36m"      /* Cyan */
#define WHITE		"\033[37m"      /* White */
#define BOLDBLACK	"\033[1m\033[30m"      /* Bold Black */
#define BOLDRED		"\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN	"\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW	"\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE	"\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA	"\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN	"\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE	"\033[1m\033[37m"      /* Bold White */