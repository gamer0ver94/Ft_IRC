#include "../h/Server.hpp"

Server::Server(int socket) : _port(socket)
{
    std::cout << "Server created with this socket : " << _port << std::endl;
    this->start();
}

Server::~Server(){}

void Server::start()
{
    struct sockaddr_in serverAddr;

    // Create server socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return;
    }

    // Set server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);  // IRC default port
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to bind socket." << std::endl;
        return;
    }

    // Listen for incoming connections
    listen(_socket, MAX_CLIENTS);
    std::cout << "Server listening on port " << _port << "..." << std::endl;

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        // Accept new client connection
        int client = accept(_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (client < 0) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        // Add the client socket to the list
        _client.push_back(client);

        std::cout << "New client connected." << std::endl;

        // Handle client communication in a separate function
        handleClient(client);
    }
}


void Server::stop()
{

}

void Server::handleClient(int client)
{

}