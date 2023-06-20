#include "../classes/CommandHandler.hpp"

void CommandHandler::handleCommand(Server& server, int &clientFd, std::string message, std::string &response){
    int messageType = getMessageType(message);
    int sendStatus;
    Client* client = NULL;
    std::string realName;
    std::string serverHostName;
    std::string hostName;
    std::string userName;
    std::string nickName;
    std::string password;
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string>words;
    std::string sendernickName;
    std::string messageContent;
    std::string channelName;
	std::string test;
    std::string response2;
    std::vector<Channel>::iterator it;
    std::map<std::string, Client>::iterator iter;
    switch (messageType){
        case 0 :    //CommandCAP
             response = handleCapabilityNegotiation(message);
            break;
        case 1 :    //CommandNICK/USER
            parseNickNameMessage(message, nickName, userName, hostName, serverHostName, realName, password);
            std::cout << server.password << "  " << password << std::endl;
            if (password.empty() || server.password != password){
                response = "ERROR :Connection refused: Password Does Not Match\r\n";
                return;
            }
            for (std::map<int, Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it){
                if (it->second->nickName == nickName){
                    response = "ERROR :Connection refused: NickName already taken\r\n";
                    return;
                }
            }
            client = new Client(clientFd, nickName, userName, hostName, serverHostName, realName);
            server.clients.insert(std::pair<int, Client*>(clientFd, client));
            response = READFILE(server.hostName, server.clients[clientFd]->nickName, client->nickName);
            break;
        case 2 :    //CommandNICK
            while (iss >> word) {
                words.push_back(word);
            }
            server.clients[clientFd]->nickName = words[1];
            response = NAMECHANGE(server.hostName, server.clients[clientFd]->nickName);
            break;
        case 3 :
           response = LOCALHOST(server.hostName, server.clients[clientFd]->nickName);
           break;
        case 4 :
            response = ":localhost CAP * ACK :END\r\n";
            break;
        case 5 :
            response = "PONG " + server.hostName + "\r\n";
            server.printData();
            break;
        case 6 :
            response = DEUX(server.hostName, server.clients[clientFd]->nickName);
            break;
        case 7 :
            response = handleCapabilityNegotiation(message);
            break;
        case 8 : // Command Join
            parseChannelName(message, channelName);
            if (!doesChannelExist(server.channels, channelName)){
                 std::cout << Cyan << "###" << server.clients[clientFd]->nickName << " created :" << channelName << "###" << Reset << std::endl;
                server.channels.push_back(Channel(channelName, *server.clients[clientFd]));
                // Search the channel end atribute the op to the client who created the channel
                for(std::vector<Channel>::iterator it = server.channels.begin(); it != server.channels.end(); ++it){
                    if (it->channelName == channelName){
                        it->opClientFd.push_back(clientFd);
                    }
                }
            }
            else{
                std::cout << Cyan << "###" << server.clients[clientFd]->nickName << " joined :" << channelName << "###" << Reset << std::endl;
            }
            for(it = server.channels.begin(); it != server.channels.end();++it){
	    		if ((*it).channelName == channelName){
	    			(*it).invitedClients.insert(std::make_pair<std::string, Client>(server.clients[clientFd]->nickName, *server.clients[clientFd]));
	    		}
	    	}
            response2 = ":" + server.clients[clientFd]->nickName + "!~" + server.clients[clientFd]->userName + "@" + server.hostName + " JOIN " + channelName + "\r\n";
            response = ":" + server.hostName + " 332 " + server.clients[clientFd]->nickName + " " + channelName + " :Welcome to " + channelName + "\r\n";
            sendStatus = send (clientFd, response2.c_str(), response2.length(), 0);
            if (sendStatus == -1){
                std::cout << "error sendin nick" << std::endl;
            }
            else if (sendStatus > 0){
	    	    std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
            }
            break;
        case 9 : // Command Part
        //send this message to all client after deleting this client of the channel
            parseChannelName(message, channelName);
            nickName = server.clients[clientFd]->nickName;
            response = ":" + nickName + " PART " + channelName + "\r\n";
            for (it = server.channels.begin();it != server.channels.end(); ++it){
                if ((*it).channelName == channelName){
                    (*it).invitedClients.erase(nickName);
                    for(iter = (*it).invitedClients.begin(); iter != (*it).invitedClients.end();++iter){
                        int sendStatus = send(iter->second.socketFd,response.c_str(),response.length(), 0);
                        if (sendStatus <= 0){
                            std::cout << Red << "Failed to send message" << Reset << std::endl;
                        }
                        else if (sendStatus > 0){
                            std::cout << Cyan<< server.clients[clientFd]->nickName << " from channel -> " << channelName << " sended: " << messageContent << "to " << iter->second.nickName << Reset << std::endl;
                        }
                    }
                    if ((*it).invitedClients.empty()) {
                        it = server.channels.erase(it);
                        --it;
                    }
                }
            }
            response = PARTA(nickName, server.hostName, channelName);
            break;
        case 10 :
            parseMessage(message, channelName, messageContent);
            for (it = server.channels.begin();it != server.channels.end(); ++it){
                if ((*it).channelName == channelName){
                    for(iter = (*it).invitedClients.begin(); iter != (*it).invitedClients.end();++iter){
                        if (iter->second.nickName == server.clients[clientFd]->nickName){
                            //skip
                        }
                        else{
	    				    response = PRIVMSG(server.clients[clientFd]->nickName, server.clients[clientFd]->userName, channelName, messageContent);
                            int sendStatus = send(iter->second.socketFd,response.c_str(),response.length(), 0);
                            if (sendStatus <= 0){
                                std::cout << Red << "Failed to send message" << Reset << std::endl;
                            }
                            else if (sendStatus > 0){
                                std::cout << Cyan<< server.clients[clientFd]->nickName << " from channel -> " << channelName << " sended: " << messageContent << "to " << iter->second.nickName << Reset << std::endl;
                            }
					        if(iter->second.socketFd < 0){
						        std::cerr << "socket does not exit" << std::endl;
	    				    }
                        }
                    }
                }
            }
            response = NOTICE(server.hostName, channelName, "Message sended succefully");
            break;
        case 11 :
            response = ":QUIT :leaving\r\n";
            for (std::map<int, Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it){
                if (it->second->nickName == nickName){
                   server.clients.erase(it);
                }
            }
            break;
        case 12 : // Command Kick
            parseKickMessage(message, channelName ,nickName);
            response = PART(nickName, channelName);
            for (it = server.channels.begin();it != server.channels.end(); ++it){
                if ((*it).channelName == channelName){
                    (*it).invitedClients.erase(nickName);
                    for(iter = (*it).invitedClients.begin(); iter != (*it).invitedClients.end();++iter){
                        int sendStatus = send(iter->second.socketFd,response.c_str(),response.length(), 0);
                        if (sendStatus <= 0){
                            std::cout << Red << "Failed to send message" << Reset << std::endl;
                        }
                        else if (sendStatus > 0){
                            std::cout << Cyan<< server.clients[clientFd]->nickName << " from channel -> " << channelName << " sended: " << messageContent << "to " << iter->second.nickName << Reset << std::endl;
                        }
                    }
                    if ((*it).invitedClients.empty()) {
                        it = server.channels.erase(it);
                        --it;
                    }
                }
            }
            response = PARTNICK(nickName, server.hostName, channelName);
            break;
        case 13 : // Command Invite
            break;
        case 14 : // Command Topic
            break;
        case 15 : // Command Mode
            break;
        default :
            response = "Unknown command.\r\n";
            break;
    }
}

bool CommandHandler::parseChannelName(const std::string& message, std::string& channelName)
{
    // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        words.push_back(word);
    }
    // Extract the nickName and username
    channelName = words[1];
    return true;
}

bool CommandHandler::parseMessage(std::string message, std::string &channelName, std::string &messageContent){
     // Find the position of the channel
   std::size_t channelStartPos = message.find("#");
    std::size_t channelEndPos = message.find(" ", channelStartPos);
    channelName = message.substr(channelStartPos, channelEndPos - channelStartPos);
	// std::cout << "testomg " << channelName << std::endl;
    // Find the position of the message
    std::size_t messageStartPos = message.find(":", channelEndPos);
    messageContent = message.substr(messageStartPos + 1);
    return true;
}
//Reads a file and output a string
std::string CommandHandler::readFile(const std::string& filePath) {
    std::ifstream file(filePath.c_str());
    std::stringstream buffer;
    
    if (file) {
        buffer << file.rdbuf();
        file.close();
    }
    
    return buffer.str();
}

bool CommandHandler::parseNickNameMessage(const std::string& message, std::string& nickName, std::string& username, std::string& hostName, std::string&serverHostName, std::string& realName, std::string &password){
    // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        words.push_back(word);
    }
    for (size_t i = 0; i < words.size(); i++){
        if (words[i] == "PASS"){
            password = words[i + 1];
        }
        else if (words[i] == "NICK"){
            nickName = words[i + 1];
        }
        else if (words[i] == "USER"){
            username = words[i + 1];
            hostName = words[i + 2];
            serverHostName = words[i + 3];
            realName = words[i + 4];
            return true;
        }
    }
    return false;
}

int CommandHandler::getMessageType(std::string message){
    if (message.find("CAP LS\r\n ") != std::string::npos){
        return 0;
    }
    else if (message.find("NICK ") != std::string::npos && message.find("USER ") != std::string::npos){
        return 1;
    }
    else if (message.find("NICK ") != std::string::npos){
        return 2;
    }
    else if (message.find("WHOIS ") != std::string::npos){
        return 3;
    }
    else if (message.find("CAP END\r\n") != std::string::npos){
        return 4;
    }
    else if (message.find("PING ") != std::string::npos){
        return 5;
    }
    else if (message.find("MODE ") != std::string::npos){
        return 6;
    }
    else if(message.find("CAP ") != std::string::npos){
        return 7;
    }
    else if (message.find("JOIN ") != std::string::npos){
        return 8;
    }
    else if (message.find("PART ") != std::string::npos){
        return 9;
    }
    else if (message.find("PRIVMSG ") != std::string::npos){
        return 10;
    }
    else if (message.find("QUIT ") != std::string::npos){
        return 11;
    }
    else if (message.find("KICK ") != std::string::npos){
        return 12;
    }
    else if (message.find("INVITE ") != std::string::npos){
        return 13;
    }
    else if (message.find("TOPIC ") != std::string::npos){
        return 14;
    }
    else if (message.find("MODE ") != std::string::npos){
        return 15;
    }
    return -1;
}

std::string CommandHandler::handleCapabilityNegotiation(const std::string& message) {
    // Check if the received message is CAP LS
    if (message.substr(0, 8) != "CAP LS\r\n")
        return "Unknown command.";
    // Prepare the list of supported capabilities
    std::string capabilities = "CAP LIST";
    // Construct the response
    std::string response = "CAP * LS :" + capabilities + "\r\n";
    return response;
}

bool CommandHandler::doesChannelExist(std::vector<Channel> &channels, std::string channelName) {
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->channelName == channelName) {
            return true;
        }
    }
    return false;
}

bool CommandHandler::parseKickMessage(std::string message, std::string &channelName, std::string &nickName){
     // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        words.push_back(word);
    }
    for (size_t i = 0; i < words.size(); i++){
        if (words[i] == "KICK"){
            channelName = words[i + 1];
            nickName = words[i + 2];
        }
    }
    return false;
}
