#pragma once
#include <iostream>
#include <unistd.h>
class Client{
	public :
		Client(int socket, std::string nickName, std::string userName, std::string hostName, std::string serverHostName, std::string realName);
		~Client();
	// Getters
		int getSocketFd();
		std::string getNickName();
		std::string getUserName();
		std::string getHostName();
		std::string getServerHostName();
		std::string getRealName();
		std::string getUserHost();
		std::string getLocalHost();
		bool getOperator();
	//Setters
		void setNickname(std::string);
		void setSocketFd(int socket);
		void setOperator(bool status);
	private :
		int socketFd;
		std::string nickName;
		std::string userName;
		std::string hostName;
		std::string serverHostName;
		std::string realName;
        std::string userHost;
        std::string localHost;
		bool op;
};