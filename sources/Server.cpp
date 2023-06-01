#include "../classes/Server.hpp"

Server::Server(std::string port, std::string password) :
port(atoi(port.c_str())), password(password), socketFd(0){
	std::cout << "Server Constructor" << std::endl;
}

Server::~Server(){
}

void Server::run(){
	std::cout << "<running server>" << std::endl;
	createSocket();
}

void Server::createSocket(){
	std::cout << "<creating socket>" << std::endl;
	socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd == -1){
		throw ServerException();
	}
	int reuseAddr = 1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1) {
        close(socketFd);
        throw std::runtime_error("Failed to set SO_REUSEADDR option.");
    }
	bindSocket();
	listening();
}
void Server::bindSocket(){
	try{
		std::cout << "<binding socket>" << std::endl;
		socketAddr.sin_family = AF_INET;
    	socketAddr.sin_port = htons(port);
    	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		int bindStatus = bind(socketFd, (struct sockaddr*)&socketAddr, sizeof(socketAddr));
		if (bindStatus == -1){
			throw std::runtime_error("Failed to get bind socket.");
		}
		struct sockaddr_in actualAddr;
    	socklen_t actualAddrLen = sizeof(actualAddr);
    	if (getsockname(socketFd, (struct sockaddr*)&actualAddr, &actualAddrLen) == -1) {
        	throw std::runtime_error("Failed to get socket name.");
    	}
	}
	catch(std::exception &e){
		std::cout << e.what() << std::endl;
	}
}

void Server::listening(){
	std::cout << "<listening socket>" << std::endl;
	listen(socketFd, 5);
	std::cout << inet_ntoa(socketAddr.sin_addr) << " port " << port << std::endl;
	while (true) {
    // Accept a new client connection
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocketFd = accept(socketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
	if (clientSocketFd == -1){
		throw std::runtime_error("Failed to accept a client connection.");
	}
    // Handle the client connection
    // You can create a new thread or use asynchronous I/O for concurrent handling of multiple clients
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocketFd, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        throw std::runtime_error("Failed to receive.");
    } else if (bytesRead == 0) {
        // Connection closed by the client
		std::cout << buffer << std::endl;
        close(clientSocketFd);
        // continue;  // Go back to accepting new connections
    }
    const char* response = "Response from server";
    ssize_t bytesSent = send(clientSocketFd, response, strlen(response), 0);
    if (bytesSent == -1) {
        throw std::runtime_error("Failed to send.");
    }

    // Close the client socket after finishing the request
    close(clientSocketFd);
}
}
// Exception

const char* Server::ServerException::what() throw() {
    return "Exception Error";
}