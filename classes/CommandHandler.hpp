#pragma once
#include <iostream>
#include "../classes/Server.hpp"

class Server;

class CommandHandler{
    public:
        static std::string handleCommand(Server& server, std::string message, std::string &response);
    private:
        std::string readFile(const std::string& filePath);
        bool parseChannelName(const std::string& message, std::string& channelName);
        bool parseMessage(std::string message, std::string &channelName, std::string &messageContent);
        bool parseNickNameMessage(const std::string& message, std::string& nickName, std::string& username, std::string& hostName, std::string&serverHostName, std::string& realName);
};