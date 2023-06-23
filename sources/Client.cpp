#include "../classes/Client.hpp"

Client::Client(int socket, std::string nickName, std::string userName, std::string hostName, std::string serverHostName, std::string realName)
: socketFd(socket), nickName(nickName), userName(userName), hostName(hostName), serverHostName(serverHostName), realName(realName), op(false){
	std::cout << Yellow << "Client Created" << Reset << std::endl;
}
Client::~Client(){
	// close(socketFd);
}
