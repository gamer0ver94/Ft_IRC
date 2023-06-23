#pragma once
#include <iostream>
#include "../classes/Server.hpp"
#include "../classes/Client.hpp"
#define CANTJOIN(servername,client,channelname) (":" + servername + " 437 " + client)
#define NOTOPERATOR(SERVERNAME, CHANNELNAME) (":" + SERVERNAME + " 482 " + SERVERNAME + " " + CHANNELNAME + " :You're not a channel operator\r\n")
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
        static bool parseKickMessage(std::string message, std::string &channelName, std::string &nickName);
		static bool isInChannel(int clientFd, Channel &channel);
		static bool isOperator(std::vector<int> operators, int clientFd);
        static std::string extractMode(std::string &message, std::string &channelName);
        static Channel& getChannelByName(std::vector<Channel> &channels, std::string channelName);
        static Client& getClientByName(std::map<int, Client*> &clients, std::string clientName);
        static std::string extractChannelPassword(std:: string &message);
        static std::string extractInvitedClient(std:: string &message);
        static std::string extractTopic(std::string message);
        static void messageAllChannelClients(Channel channel, int client, std::string message);
        static void extractNewOp(std::string message, std::string &nickName, std::string &channelName);
        static void removeElementByFd(std::vector<int>& vec, int id);
};      
// get channel by name
