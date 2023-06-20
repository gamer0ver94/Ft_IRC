#include "../classes/Channel.hpp"
#include "../classes/Server.hpp"
Channel::Channel(std::string name, Client op) :
_channelName(name){
	std::cout << Green << "Channel " << getChannelName() << " created!" << Reset << std::endl;
	(void)op;
}
Channel::~Channel(){
}

std::string Channel::getChannelName()
{
	return (this->_channelName);
}

std::vector<int> Channel::getOpClientFd()
{
	return (this->_opClientFd);
}

std::map<std::string, Client> &Channel::getInvitedClients()
{
	return (this->_invitedClients);
}