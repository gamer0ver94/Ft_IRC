#pragma once
#include <iostream>
#include "Client.hpp"
#include <map>
#include <vector>
#include "../includes/colors.hpp"

class Client;
class Channel{
	public :
		std::string channelName;
		std::map<std::string, Client> invitedClients;
		Channel(std::string name, Client op);
		~Channel();
		std::vector<int>opClientFd;
		std::vector<int>invitedClientsToChannel;
		bool iMode;
		bool tMode;
		bool kMode;
		bool oMode;
		std::string password;
		std::string topic;
	private :
};