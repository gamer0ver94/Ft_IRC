#include "../classes/Server.hpp"
#include <csignal>
extern bool test;
Server::Server(unsigned int port, std::string password) :
    port(port), password(password), socketFd(0), hostName("IRCSERVER") {
    std::cout << "Server Constructor" << std::endl;
}

Server::~Server() {
}

void Server::run() {
    std::cout << Green << "Server Running ..." << Reset<< std::endl;
    createSocket();
}

void Server::createSocket() {
    std::cout << Yellow << "Creating Server Socket" << Reset << std::endl;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        throw std::runtime_error("Failed to create socket.");
    }
    int reuseAddr = 1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseAddr, sizeof(reuseAddr)) == -1) {
        close(socketFd);
        throw std::runtime_error("Failed to set SO_REUSEADDR option.");
    }
    //testing to see if changes
   if(fcntl(socketFd, F_SETFL, O_NONBLOCK)){
        std::cout << "error fcntl " << std::endl;
   }
    bindSocket();
    listening();
}

void Server::bindSocket() {
    try {
        struct sockaddr_in actualAddr;
        socklen_t actualAddrLen = sizeof(actualAddr);
        // Initialize Struct socketAddr
        socketAddr.sin_family = AF_INET;
        socketAddr.sin_port = htons(port);
        socketAddr.sin_addr.s_addr = htons(INADDR_ANY);
        //Give server new name baseon ip
        // Bind the Socket
        int bindStatus = bind(socketFd, (struct sockaddr*)&socketAddr, sizeof(socketAddr));
        if (bindStatus == -1) {
            throw std::runtime_error("Failed to bind socket.");
        }
        // Get Socket Name
        if (getsockname(socketFd, (struct sockaddr*)&actualAddr, &actualAddrLen) == -1) {
            throw std::runtime_error("Failed to get socket name.");
        }
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

void Server::listening() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    listen(socketFd, 5);
    std::cout << Green << "Listening Clients Connections ..." << Reset << std::endl;
    // Create a vector of pollfd structures to hold file descriptors and events
    pollfd serverPollFd;
    serverPollFd.fd = socketFd;
    serverPollFd.events = POLLIN;
    serverPollFd.revents = 0;
    pollFds.push_back(serverPollFd);
    while (true) {
        if (test){
            printData();
        }
        // Wait for activity on any of the monitored file descriptors
        int pollResult = poll(pollFds.data(), pollFds.size(), -1);
        if (pollResult == -1) {
            throw std::runtime_error("Error in poll system call.");
        }
        // Check if there is a new client connection   
        if (pollResult >= 0){
            if (pollFds[0].revents & (POLLIN)) {
                int clientSocket = accept(socketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
                if (clientSocket == -1) {
                    throw std::runtime_error("Failed to accept a client connection.");
                }
                // Add the new client socket to the vector of pollfd structures
                pollfd clientPollFd;
                clientPollFd.fd = clientSocket;
                clientPollFd.events = POLLIN;
                clientPollFd.revents = 0;
                pollFds.push_back(clientPollFd);
            }
        }
        handleCommunication(pollFds);
    }
}

void Server::handleCommunication(std::vector<pollfd>& pollFds) {
    // Iterate through all the file descriptors in the vector
    for (int i = pollFds.size() - 1; i >= 1; --i) {
        if (pollFds[i].revents & POLLIN) {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            // Receive Data from Client
            int recvBytes = recv(pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);
            if (recvBytes < 0) {
                throw std::runtime_error("Failed to receive data from client.");
            }
            else if (recvBytes == 0) {
                // Client closed the connection
                close(pollFds[i].fd);
                pollFds.erase(pollFds.begin() + i);
                // --i;
                continue;
            }
            else {
                buffer[recvBytes] = '\0';
                std::string message(buffer);
                handleClientMessage(message, pollFds[i].fd);
            }
        }
    }
}

void Server::handleClientMessage(std::string message, int& clientFd){
    int sendingStatus;
    std::cout << Magenta << "______________________________________________________________" << Reset << std::endl;
    std::cout << Green << "=> Received Data From Client: " << Reset << message << std::endl;
    std::string response;
    CommandHandler::handleCommand(*this, clientFd, message, response);
    // If there is a connection refuse then delete the fd of the client
    if (response == "ERROR :Connection refused: NickName already taken"){
        for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it){
            if (it->fd == clientFd){
                // it = pollFds.erase(it);
				pollFds.erase(it);
                //  --it;
            }
        }
    }
    if (response == "QUIT :leaving\r\n")
    {
		// Remove filedescriptor if client send QUIT message
       for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it){
            if (it->fd == clientFd){
                //  it = pollFds.erase(it);
				pollFds.erase(it);
                //  --it;
            }
        } 
    }
    sendingStatus = send(clientFd, response.c_str(), response.length(), 0);
    if (sendingStatus == -1){
        std::cout << Red << "Error Sending response from the server" << Reset << std::endl;
    }
    else{
        std::cout << Blue << "=> Server Sended Response with: " << Reset << response;
    }
}

void Server::printData(){
    for (std::vector<Channel>::iterator it = channels.begin();it != channels.end(); ++it){
        std::cout << "Channel: " << it->channelName << std::endl;
        std::cout << "i Mode = " << it->iMode << std::endl;
        std::cout << "t Mode = " << it->tMode << std::endl;
        std::cout << "k Mode = " << it->kMode << std::endl;
        std::cout << "o Mode = " << it->oMode << std::endl;
        for(std::map<std::string, Client>::iterator iter = (*it).invitedClients.begin(); iter != (*it).invitedClients.end();++iter){ 
           //isOperator(it->opClientFd, iter->second.socketFd);
            if (it->opClientFd[0] == iter->second.socketFd){
                std::cout << "Client op: " << iter->second.nickName << std::endl;
            }
            else{
                std::cout << "Client: " << iter->second.nickName << std::endl;
            }
        }
        if (!it->password.empty()){
             std::cout << "Channel Password: " << it->password << std::endl;
        }
        std::cout << "Topic " << it->topic << std::endl;
        std::cout << "_______________________________" << std::endl;
    }
}