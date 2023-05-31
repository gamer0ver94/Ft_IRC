#pragma once
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

class Server{
	public :
		// Constructors
		Server(std::string port, std::string password);
		~Server();
		// Methodes
		void run();
		class	ServerException{
			public :
				const char* what() throw();
		};
	private :
		// Properties
		unsigned int port;
		std::string password;
		int socketFd;
		struct sockaddr_in *socketAddr;
		// Methodes
		void createSocket();
		void bindSocket();
};