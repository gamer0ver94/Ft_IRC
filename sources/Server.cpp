#include "../classes/Server.hpp"

Server::Server(unsigned int port, std::string password) :
    port(port), password(password), socketFd(0), hostName("localhost") {
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
        hostName = inet_ntoa(socketAddr.sin_addr);
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
    //std::vector<pollfd> pollFds = getPollfds(); getter pollFds
    socklen_t clientAddrLen = sizeof(clientAddr);
    listen(socketFd, 5);
    std::cout << Green << "Listening Clients Connections ..." << Reset << std::endl;
    // Create a vector of pollfd structures to hold file descriptors and events
    pollfd serverPollFd;
    serverPollFd.fd = socketFd;
    serverPollFd.events = POLLIN;
    serverPollFd.revents = 0;
    getPollFds().push_back(serverPollFd);
    while (true) {
        // Wait for activity on any of the monitored file descriptors
        int pollResult = poll(getPollFds().data(), getPollFds().size(), -1);
        if (pollResult == -1) {
            throw std::runtime_error("Error in poll system call.");
        }
        // Check if there is a new client connection   
        if (pollResult > 0){
            if (getPollFds()[0].revents & (POLLIN)) {
                int clientSocket = accept(socketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
                if (clientSocket == -1) {
                    throw std::runtime_error("Failed to accept a client connection.");
                }
                // Add the new client socket to the vector of pollfd structures
                pollfd clientPollFd;
                clientPollFd.fd = clientSocket;
                clientPollFd.events = POLLIN;
                clientPollFd.revents = 0;
                getPollFds().push_back(clientPollFd);
            }
        }
        handleCommunication(); //getter
    }
}

void Server::handleCommunication() {
    // Iterate through all the file descriptors in the vector
    //std::vector<pollfd> pollFds = getPollfds(); //getter pollFds
    for (int i = getPollFds().size() - 1; i >= 1; --i) {
        if (getPollFds()[i].revents & POLLIN) {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            // Receive Data from Client
            int recvBytes = recv(getPollFds()[i].fd, buffer, sizeof(buffer) - 1, 0);
            if (recvBytes < 0) {
                throw std::runtime_error("Failed to receive data from client.");
            }
            else if (recvBytes == 0) {
                // Client closed the connection
                close(getPollFds()[i].fd);
                getPollFds().erase(getPollFds().begin() + i); 
                // --i;
                continue;
            }
            else {
                buffer[recvBytes] = '\0';
                std::string message(buffer);
                handleClientMessage(message, getPollFds()[i].fd);
            }
        }
    }
}


void Server::handleClientMessage(std::string message, int& clientFd){
    int sendingStatus;
    //std::vector<pollfd> pollFds = getPollfds(); getter pollFds
    std::cout << Green << "=> Received Data From Client: " << Reset << message << std::endl;
    std::string response;
    CommandHandler pp;
    pp.handleCommand(*this, clientFd, message, response);
    //CommandHandler::handleCommand(*this, clientFd, message, response);
    // If there is a connection refuse then delete the fd of the client
    if (response == "ERROR :Connection refused: NickName already taken"){
        for (std::vector<pollfd>::iterator it = getPollFds().begin(); it != getPollFds().end(); ++it){
            if (it->fd == clientFd){
                // it = pollFds.erase(it);
				getPollFds().erase(it);
                //  --it;
            }
        }
    }
    if (response == "QUIT :leaving\r\n")
    {
		// Remove filedescriptor if client send QUIT message
       for (std::vector<pollfd>::iterator it = getPollFds().begin(); it != getPollFds().end(); ++it){
            if (it->fd == clientFd){
                //  it = pollFds.erase(it);
				getPollFds().erase(it);
                //  --it;
            }
        } 
    }
    sendingStatus = send(clientFd, response.c_str(), response.length(), 0);
    if (sendingStatus == -1){
        std::cout << Red << "Error Sending response from the server" << Reset << std::endl;
    }
    else{
        std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
    }
}

void Server::printData(void){
    for (std::vector<Channel>::iterator it = getChannels().begin();it != getChannels().end(); ++it){
        std::cout << "Channel: " << it->getChannelName() << std::endl;
        std::cout << "i Mode = " << it->iMode << std::endl;
        std::cout << "t Mode = " << it->tMode << std::endl;
        std::cout << "k Mode = " << it->kMode << std::endl;
        std::cout << "o Mode = " << it->oMode << std::endl;
        for(std::map<std::string, Client>::iterator iter = (*it).getInvitedClients().begin(); iter != (*it).getInvitedClients().end();++iter){ 
            if (it->getOpClientFd()[0] == iter->second.getSocketFd()){
                std::cout << "Client op: " << iter->second.getNickName() << std::endl;
            }
            else{
                std::cout << "Client: " << iter->second.getNickName() << std::endl;
            }
        }
        if (!it->password.empty()){
             std::cout << "Channel Password: " << it->password << std::endl;
        }
        std::cout << "Topic " << it->topic << std::endl;
        std::cout << "_______________________________" << std::endl;
    }
}

std::vector<pollfd> &Server::getPollFds()
{
    return (this->_pollFds);
}

unsigned int Server::getPort()
{
    return (this->port);
}

std::string Server::getPassword()
{
    return (this->password);
}

int Server::getSocketFd()
{
    return (this->socketFd);
}

std::string Server::getHostName()
{
    return (this->hostName);
}

std::map<int, Client*> &Server::getClients()
{
    return (this->_clients);
}

std::vector<Channel> &Server::getChannels()
{
    return (this->_channels);
}