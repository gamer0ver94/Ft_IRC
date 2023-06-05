#pragma once
#include <iostream>
#include <unistd.h>
class Client{
	public :
		Client(std::string nickName, int socket);
		~Client();
	// Getters
		// std::string getNickname();
		// int getSocketFd;
		// bool getOperator();
	//Setters
		// void setNickname(std::string);
		// void setSocketFd(int socket);
		// void setOperator(bool status);
		std::string nickname;
		int socketFd;
	private :
        std::string userHost;
        std::string localHost;
		bool op;
};