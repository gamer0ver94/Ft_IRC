#include "../classes/Server.hpp"

Server::Server(std::string port, std::string password) :
port(std::stoi(port)), password(password), socketFd(0), socketAddr(NULL){
	std::cout << "Server Constructor" << std::endl;
}

Server::~Server(){
	delete socketAddr;
}

void Server::run(){
	std::cout << "run" << std::endl;
}

void Server::createSocket(){
	socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd == -1){
		throw ServerException();
	}
	bindSocket();

}
void Server::bindSocket(){
	socketAddr = new sockaddr_in();
	struct hostent* localHost = gethostbyname("");
	char *localIp = inet_ntoa(*reinterpret_cast<in_addr*>(localHost->h_addr_list));
	socketAddr->sin_family = AF_INET;
	socketAddr->sin_addr.s_addr = inet_addr(localIp);
	socketAddr->sin_port = htons(port);
	int bindStatus = bind(socketFd, reinterpret_cast<sockaddr*>(&socketAddr), sizeof(socketAddr));
	if (bindStatus == -1){
		throw ServerException();
	}
}

// Exception

const char* Server::ServerException::what() throw() {
    return "Exception Error";
}