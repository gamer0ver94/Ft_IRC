#pragma once
#include <iostream>
#include "Client.hpp"
#include <map>
class Client;
class Channel{
	public :
		std::string channelName;
		Channel(std::string name, Client op);
		~Channel();
		std::map<std::string, Client> invitedClients;
	private :
		Client opClient;
};