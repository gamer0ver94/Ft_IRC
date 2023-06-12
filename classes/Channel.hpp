#pragma once
#include <iostream>
#include "Client.hpp"
#include <map>
#include <vector>
class Client;
class Channel{
	public :
		std::string channelName;
		std::map<std::string, Client> invitedClients;
		Channel(std::string name, Client op);
		~Channel();
		std::vector<int>opClientFd;
		bool iMode;
		bool tMode;
		bool kMode;
		std::string password;
		std::string topic;
	private :
};