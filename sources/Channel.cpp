#include "../classes/Channel.hpp"
#include "../classes/Server.hpp"
Channel::Channel(std::string name, Client op) :
channelName(name), iMode(false), tMode(false), kMode(false), oMode(false){
	std::cout << Green << "Channel " << channelName << " created!" << Reset << std::endl;
	(void)op;
}
Channel::~Channel(){
}