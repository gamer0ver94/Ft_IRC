#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <vector>

const int MAX_CLIENTS = 10;

class Server
{
    public :
            Server();
            Server(int);
            Server(const Server &);
            Server &operator=(const Server &);
            ~Server();

            void start();
            void stop();
            void handleClient(int);



    private :
            int _socket;
            int _port;
            std::vector<int> _client;
            std::vector<std::string> _channel;

};