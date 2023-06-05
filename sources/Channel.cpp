#include "../classes/Channel.hpp"
#include "../classes/Server.hpp"
Channel::Channel(std::string name, Client op) : channelName(name), opClient(op){
	std::cout << Green << "Channel " << channelName << " created!" << Reset << std::endl;
}
Channel::~Channel(){
}