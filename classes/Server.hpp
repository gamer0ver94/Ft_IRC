#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/poll.h>
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>
#include "Channel.hpp"
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
// Text Colors
#define Black "\033[30m"
#define Red "\033[31m"
#define Green "\033[32m"
#define Yellow "\033[33m"
#define Blue "\033[34m"
#define Magenta "\033[35m"
#define Cyan "\033[36m"
#define White "\033[37m"
// Background Colors
#define BgBlack "\033[40m"
#define BgRed "\033[41m"
#define BgGreen "\033[42m"
#define BgYellow "\033[43m"
#define BgBlue "\033[44m"
#define BgMagenta "\033[45m"
#define BgCyan "\033[46m"
#define BgWhite "\033[47m"
// Reset color
#define Reset "\033[0m"
#include "../classes/CommandHandler.hpp"

class Server{
	public :
		// Constructors
		Server(unsigned int port, std::string password);
		~Server();
		// Methodes
		void run();
		unsigned int port;
		std::string password;
		int socketFd;
		std::string hostName;
		std::vector<pollfd> pollFds;
		std::map<int, Client*> clients;
		std::vector<Channel> channels;
		void printData(void);
	private :
		// Properties
		struct sockaddr_in socketAddr;
		// Methodes
		void createSocket();
		void bindSocket();
		void listening();
		std::string readFile(const std::string& filePath);
		void handleCommunication(std::vector<pollfd>& pollFds);
		void handleClientMessage(std::string message, int& clientFd);
};
