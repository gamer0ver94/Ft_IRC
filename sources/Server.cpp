#include "../classes/Server.hpp"

Server::Server(unsigned int port, std::string password) :
    port(port), password(password), socketFd(0) {
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
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const char*)&reuseAddr, sizeof(reuseAddr)) == -1) {
        close(socketFd);
        throw std::runtime_error("Failed to set SO_REUSEADDR option.");
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
    // Accept a new client connection
        int clientSocket = accept(socketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
        if (clientSocket == -1) {
            throw std::runtime_error("Failed to accept a client connection.");
        }
        // handleCommunication
        while (true){
            handleCommunication(clientSocket);
        }
}

void Server::handleCommunication(int clientSocket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    // Receive Data from Client
    int recvBytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (recvBytes == -1) {
        throw std::runtime_error("Failed to receive data from client.");
    }
    buffer[recvBytes] = '\0';
    std::string message(buffer);
    std::cout << Green << "Received Data From Client: " << Reset << message << std::endl;
    std::string response;
    if (message.substr(0, 5) == "NICK " && message.find("USER ") != std::string::npos) {
        // Handle the user information and connection establishment
        // ...

        // Send a welcome message to the client
        std::string welcomeMessage = ":SERVER 001 WELCOME :Welcome to the IRC server\r\n";
        int sendStatus = send(clientSocket, welcomeMessage.c_str(), welcomeMessage.length(), 0);
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.substr(0, 4) == "CAP ") {
        // Handle capability negotiation commands
        response = handleCapabilityNegotiation(message);
        std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;

    } else if (message.substr(0, 5) == "JOIN ") {
        // Handle join channel commands
        // response = handleJoinChannel(message);
    } else if (message.substr(0, 4) == "PART ") {
        // Handle leave channel commands
        // response = handleLeaveChannel(message);
    } else {
        // Handle unrecognized commands
        response = "Unknown command.";
    }
    // Print Data
}

std::string Server::handleCapabilityNegotiation(const std::string& message) {
    // Check if the received message is CAP LS
    if (message.substr(0, 8) != "CAP LS\r\n")
        return "Unknown command.";
    // Prepare the list of supported capabilities
    std::string capabilities = "multi-prefix message-tags account-notify extended-join";
    // Construct the response
    std::string response = "CAP * LS :" + capabilities + "\r\n";
    return response;
}
