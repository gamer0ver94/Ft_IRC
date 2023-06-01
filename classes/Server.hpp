#pragma once
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <vector>
#include "Channel.hpp"

class Server{
	public :
		// Constructors
		Server(std::string port, std::string password);
		~Server();
		// Methodes
		void run();
	private :
		// Properties
		unsigned int port;
		std::string password;
		int socketFd;
		struct sockaddr_in socketAddr;
		// Methodes
		void createSocket();
		void bindSocket();
		void listening();
		std::vector<Client> clients;
		std::vector<Channel> channels;
};
