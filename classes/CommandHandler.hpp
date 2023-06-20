#pragma once
#include <iostream>
#include "../classes/Server.hpp"
#include "../classes/Client.hpp"


#define READFILE(host, clientFD, clientNick) (":" + host + " 001 " + clientFD + " :" +  readFile("wel.txt")  + " " + clientNick + " " + readFile("come.txt") + "\r\n")
#define NAMECHANGE(host, clientFD) (":" + host + " 001 " + clientFD + " :Name changed to " + clientFD + "\r\n")
#define LOCALHOST(host, clientFD) (":" + host + " 311 " + clientFD + " localhost " + clientFD + " *\r\n")
#define DEUX(host, clientFD) (":" + host + " 221" + clientFD + " -I\r\n")
#define PARTA(nick, host, channel) (":" + nick + "!" + nick + "@" + host + " Part " + channel + "\r\n")
#define PRIVMSG(clientFD, user, channel, message) (":" + clientFD + "!~" + user + " PRIVMSG " + channel + " :" + message + "\r\n")
#define NOTICE(host, channel, message) (":" + host + " NOTICE " + channel + " :" + message + "\r\n")
#define PART(nick, channel) (":" + nick + " PART " + channel + "\r\n")
#define PARTNICK(nick, host, channel) (":" + nick + "!" + nick + "@" + host + " Part " + channel + "\r\n")
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
};      

