#include "../classes/CommandHandler.hpp"

static std::string CommandHandler::handleCommand(Server& server, std::string message, std::string &response){
    switch (message){
        case : message.find("NICK ") != std::string::npos && message.find("USER ") != std::string::npos
            // Handle the user information and connection establishment
            std::string nickName;
            std::string userName;
            std::string hostName;
            std::string serverHostName;
            std::string realName;
            parseNickNameMessage(message, nickName, userName, hostName, serverHostName, realName);
            Client *client = new Client(pollFds.fd, nickName, userName, hostName, serverHostName, realName);
            clients.insert(std::pair<int, Client*>(pollFds.fd, client));
            hostName = "briths";
            std::string welcomeMessage = ":" + this->hostName + " 001 " + clients[pollFds.fd]->nickName + " :" +  readFile("wel.txt")  + " " + client->nickName + " " + readFile("come.txt") + "\r\n";
        case : message.find("NICK ") != std::string::npos
            std::istringstream iss(message);
            std::string word;
            std::vector<std::string> words;
            while (iss >> word) {
                words.push_back(word);
            }
            clients[pollFds.fd]->nickName = words[1];
            response = ":" + this->hostName + " 001 " + clients[pollFds.fd]->nickName + " :Name changed to " + clients[pollFds.fd]->nickName + "\r\n";
            int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
            std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
            if (sendStatus == -1) {
                throw std::runtime_error("Failed to send data to client.");
            }
        // case : message.find("WHOIS ") != std::string::npos
        //    response = ":" + this->hostName + " 311 " + clients[pollFds.fd]->nickName + " localhost " + clients[pollFds.fd]->nickName + " *\r\n";
        //     int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        //     std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        //     if (sendStatus == -1) {
        //         throw std::runtime_error("Failed to send data to client.");
        //     }
        // case : message.find("CAP END\r\n") != std::string::npos
        //     response = ":localhost CAP * ACK :END\r\n";
        //     int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        //     std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        //     if (sendStatus == -1) {
        //         throw std::runtime_error("Failed to send data to client.");
        //     }
        // case : message.find("PING ") != std::string::npos
        //     response = "PONG " + this->hostName + "\r\n";
        //     int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        //     std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        //     if (sendStatus == -1) {
        //         throw std::runtime_error("Failed to send data to client.");
        //     }
        // }
        // case : message.find("MODE ") != std::string::npos
        //     response = ":" + this->hostName + " 221" + clients[pollFds.fd]->nickName + " -I\r\n";
        //     std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        //     int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        //     if (sendStatus == -1) {
        //         throw std::runtime_error("Failed to send data to client.");
        //     }
        // case : message.find("CAP ") != std::string::npos
        //     response = handleCapabilityNegotiation(message);
        //     std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        //     int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        //     if (sendStatus == -1) {
        //         throw std::runtime_error("Failed to send data to client.");
        //     }
        // case : message.find("JOIN ") != std::string::npos
        //     for(std::map<int, Client*>::iterator it = clients.begin(); it != clients.end();++it){
        //     }
        //     std::string channelName;
        //     parseChannelName(message, channelName);
        //     std::cout << Cyan << "###" << clients[pollFds.fd]->nickName << " joined :" << channelName << "###" << Reset << std::endl;
        //     channels.push_back(Channel(channelName, *clients[pollFds.fd]));
        //     for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end();++it){
	    // 		if ((*it).channelName == channelName){
	    // 			(*it).invitedClients.insert(std::make_pair<std::string, Client>(clients[pollFds.fd]->nickName, *clients[pollFds.fd]));
	    // 		}
	    // 	}
        //     std::string response2 = ":" + clients[pollFds.fd]->nickName + "!~" + clients[pollFds.fd]->userName + "@" + hostName + " JOIN " + channelName + "\r\n";
        //     response = ":" + this->hostName + " 332 " + clients[pollFds.fd]->nickName + " " + channelName + " :Welcome to " + channelName + "\r\n";
        //     int sendStatus = send (pollFds.fd, response.c_str(), response.length(), 0);
        //     if (sendStatus == -1){
        //         std::cout << "error sendin nick" << std::endl;
        //     }
        //     else if (sendStatus > 0){
	    // 	    std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
        //         sendStatus = send (pollFds.fd, response2.c_str(), response2.length(), 0);
        //     }
        // case : message.find("PART ") != std::string::npos
        //     response = "PART\r\n";
        //    int sendStatus = send(pollFds.fd,response.c_str(), response.length(), 0);
        //     if (sendStatus == -1) {
        //         throw std::runtime_error("Failed to send data to client.");
        //     }
        //     else{
        //         std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
        //     }
	    // case : message.find("NICK ") != std::string::npos
	    // 	clients[pollFds.fd]->nickName = "another";
        //     response = "NICK another";
        //     int sendStatus = send (pollFds.fd, response.c_str(), response.length(), 0);
        //     if (sendStatus == -1){
        //         std::cout << "error sendin nick" << std::endl;
        //     }
        //     else if (sendStatus > 0){
	    // 	    std::cout << Yellow<< "Client changed nickname to <" << clients[pollFds.fd]->nickName << ">" << Reset<< std::endl;
        //     }
	    // }
        // case : message.substr(0, 8) == "PRIVMSG "
        //     std::string sendernickName;
        //     std::string messageContent;
        //     std::string channelName;
        //     std::string messageType;
	    // 	std::string test;
        //     parseMessage(message, channelName, messageContent);
        //     for (std::vector<Channel>::iterator it = channels.begin();it != channels.end(); ++it){
        //         if ((*it).channelName == channelName){
        //             for(std::map<std::string, Client>::iterator iter = (*it).invitedClients.begin(); iter != (*it).invitedClients.end();++iter){
        //                 if (iter->second.nickName == clients[pollFds.fd]->nickName){

        //                 }
        //                 else{
	    // 				    test = ":" + clients[pollFds.fd]->nickName + "!~" + clients[pollFds.fd]->userName + " PRIVMSG " + channelName + " :" + messageContent + "\r\n";
	    // 				     int sendStatus = send(iter->second.socketFd,test.c_str(),test.length(), 0);
        //                      if (sendStatus <= 0){
        //                         std::cout << Red << "Failed to send message" << Reset << std::endl;
        //                      }
        //                      else if (sendStatus > 0){
        //                         std::cout << Cyan<< clients[pollFds.fd]->nickName << " from channel -> " << channelName << " sended: " << messageContent << "to " << iter->second.nickName << Reset << std::endl;
        //                      }
	    // 				    if(iter->second.socketFd < 0){
	    // 					std::cerr << "socket does not exit" << std::endl;
	    // 				}
        //                 }
        //             }
        //         }
        //     }
        default :
            response = "Unknown command.\r\n";
            int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
            if (sendStatus == -1) {
                throw std::runtime_error("Failed to send data to client.");
            }
            else{
                std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
            }
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

bool CommandHandler::parseNickNameMessage(const std::string& message, std::string& nickName, std::string& username, std::string& hostName, std::string&serverHostName, std::string& realName){
    // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        words.push_back(word);
    }
    for (size_t i = 0; i < words.size(); i++){
        if (words[i] == "NICK"){
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