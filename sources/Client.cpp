#include "../classes/Client.hpp"

Client::Client(std::string nickName, int socket) : nickname(nickName), socketFd(socket), op(false){
	std::cout << "constructor" << nickname << socketFd << op  << std::endl;
}
Client::~Client(){
	// close(socketFd);
}