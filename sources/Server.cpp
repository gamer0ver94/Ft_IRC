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
    char buffer[1024];
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    std::cout << Green << "Listening Clients Connections ..." << Reset << std::endl;
    // Make Server Socket Listenning Income Clients 
    listen(socketFd, 5);
    // std::cout << inet_ntoa(socketAddr.sin_addr) << " port " << port << std::endl;
    // Accept a new client connection
    // Handle the client connection
    // You can create a new thread or use asynchronous I/O for concurrent handling of multiple clients
    while (true) {
        int clientSocket = accept(socketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
        if (clientSocket == -1) {
            throw std::runtime_error("Failed to accept a client connection.");
        }
        strcpy(buffer, ":SHITYSERVER 001 PANDA :Welcome to the IRC server");
        send(clientSocket, buffer, sizeof(buffer) + 1, 0);
        // send(clientSocket,buffer, sizeof(buffer) + 1, 0);
        memset(buffer, 0, 1024);
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            throw std::runtime_error("Failed to receive.");
        }
        else if (bytesRead == 0) {
            // Connection closed by the client
            close(clientSocket);
            break; // Go back to accepting new connections
        }
        buffer[bytesRead] = '\0';
        std::cout << "Received message: " << std::string(buffer, 0, bytesRead) << std::endl;
        
        // Process the client message and send a response
        // Handle different commands
        // Add more conditions to handle other commands
    }
        // close(clientSocket);
}