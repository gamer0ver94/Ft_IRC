#include "../classes/Channel.hpp"

Channel::Channel(std::string name, Client op) : channelName(name), opClient(op){
	std::cout << channelName << std::endl;
}
Channel::~Channel(){
}