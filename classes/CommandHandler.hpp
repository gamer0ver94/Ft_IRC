#pragma once
#include <iostream>
#include "../classes/Server.hpp"
#include "../classes/Client.hpp"

class Server;

class CommandHandler{
    public:
        static void handleCommand(Server& server, int &clientFd, std::string message, std::string &response);
    private:
        static int getMessageType(std::string message);
        static std::string handleCapabilityNegotiation(const std::string& message);
        static std::string readFile(const std::string& filePath);
        static bool parseChannelName(const std::string& message, std::string& channelName);
        static bool parseMessage(std::string message, std::string &channelName, std::string &messageContent);
        static bool parseNickNameMessage(const std::string& message, std::string& nickName, std::string& username, std::string& hostName, std::string&serverHostName, std::string& realName, std::string &password);
        static bool doesChannelExist(std::vector<Channel> &channels, std::string channelName);
};

