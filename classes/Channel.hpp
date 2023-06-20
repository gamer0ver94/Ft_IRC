#pragma once
#include <iostream>
#include "Client.hpp"
#include <map>
#include <vector>
class Client;
class Channel{
	public :
		Channel(std::string name, Client op);
		~Channel();
		//getters
		std::string getChannelName();
		std::vector<int> getOpClientFd();
		std::map<std::string, Client> &getInvitedClients();
		bool iMode;
		bool tMode;
		bool kMode;
		bool oMode;
		std::string password;
		std::string topic;
		std::vector<int>invitedClientsToChannel;
	private :
		std::string _channelName;
		std::vector<int>_opClientFd;
		std::map<std::string, Client> _invitedClients;
};