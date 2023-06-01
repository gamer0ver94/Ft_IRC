#pragma once
#include <iostream>
#include "Client.hpp"
#include <map>

class Channel{
	public :
		Channel(std::string name, Client op);
		~Channel();
	private :
		std::string channelName;
		Client opClient;
		std::map<std::string, Client> invitedClients;
};