#pragma once
#include <iostream>
#include <unistd.h>
#include "../includes/colors.hpp"
class Client{
	public :
		Client(int socket, std::string nickName, std::string userName, std::string hostName, std::string serverHostName, std::string realName);
		~Client();
	// Getters
		// std::string getNickname();
		// int getSocketFd;
		// bool getOperator();
	//Setters
		// void setNickname(std::string);
		// void setSocketFd(int socket);
		// void setOperator(bool status);
		int socketFd;
		std::string nickName;
		std::string userName;
		std::string hostName;
		std::string serverHostName;
		std::string realName;
	private :
        std::string userHost;
        std::string localHost;
		bool op;
};