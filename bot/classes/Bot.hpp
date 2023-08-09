#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/poll.h>
#include <string>
#include <sstream>
#include <fstream>
#include <string.h>
#include <ctime>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../includes/colors.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../classes/Game.hpp"
#define RULES(channelName) (std::string("PRIVMSG ") + std::string(channelName) + " Welcome, i am a bot.\nRules : to join type join game to start type start game\r\n")
#define CUSTUM_MESSAGE(channelName, message) (std::string("PRIVMSG ") + std::string(channelName) + " :" + message + "\r\n")
class Game;

class Bot{
    public :
        Bot(std::string botName, std::string serverIp, int port, std::string password);
        ~Bot();
        void run();
        //getter
        time_t staringCount();
        //setter
        void setStartingCount(time_t time);
    private :
        time_t startingCount;
        std::string botName;
        std::string serverIp;
        int port;
        std::string password;
        int socketFd;
        struct sockaddr_in socketAddr;
        void connectToServer();
        void createSocket();
        void authenticate();
        void handleMessage(std::string &message, std::string &response);
        void joinEveryChannel(std::string message);
        static void signalHandler(int signal);
        static bool running;
        std::map<std::string, Game*> game;
        bool createGame(std::string channelName, std::string clientName);
        bool joinPlayer(std::string channelName, std::string clientName);
        void getMessageInfo(std::string message, std::string &clientName, std::string &channelName, std::string &content);
        void startGame(std::string channelName);
        void extractTopicAndQuestion(const std::string& input, std::string& topic, std::string& question);
		std::string whoWonGame(std::map<std::string, int> players);
        bool isPlaying(std::string clientName, std::map<std::string, int> players);
};