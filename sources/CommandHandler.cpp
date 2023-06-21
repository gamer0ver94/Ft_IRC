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
    std::string mode;
    std::string topic;
    std::string response2;
    std::vector<Channel>::iterator it;
    std::map<std::string, Client>::iterator iter;
	std::vector<int> op;
    switch (messageType){
        case 0 :    //CommandCAP
             response = handleCapabilityNegotiation(message);
            break;
        case 1 :    //CommandNICK/USER
            parseNickNameMessage(message, nickName, userName, hostName, serverHostName, realName, password);
            std::cout << server.getPassword() << "  " << password << std::endl;
            if (password.empty() || server.getPassword() != password){
                response = "ERROR :Connection refused: Password Does Not Match\r\n";
                return;
            }
            for (std::map<int, Client*>::iterator it = server.getClients().begin(); it != server.getClients().end(); ++it){
                if (it->second->getNickName() == nickName){
                    response = "ERROR :Connection refused: NickName already taken\r\n";
                    return;
                }
            }
            client = new Client(clientFd, nickName, userName, hostName, serverHostName, realName);
            server.getClients().insert(std::pair<int, Client*>(clientFd, client));
            response = ":" + server.getHostName() + " 001 " + server.getClients()[clientFd]->getNickName() + " :" +  readFile("wel.txt")  + " " + client->getNickName() + " " + readFile("come.txt") + "\r\n";
            break;
        case 2 :    //CommandNICK
            while (iss >> word) {
                words.push_back(word);
            }
            server.getClients()[clientFd]->getNickName() = words[1];
            response = ":" + server.getHostName() + " 001 " + server.getClients()[clientFd]->getNickName() + " :Name changed to " + server.getClients()[clientFd]->getNickName() + "\r\n";
            break;
        case 3 :
           response = ":" + server.getHostName() + " 311 " + server.getClients()[clientFd]->getNickName() + " localhost " + server.getClients()[clientFd]->getNickName() + " *\r\n";
           break;
        case 4 :
            response = ":localhost CAP * ACK :END\r\n";
            break;
        case 5 :
            response = "PONG " + server.getHostName() + "\r\n";
            server.printData();
            break;
        case 6 : // Command Mode
            try{
                mode = extractMode(message, channelName);
                if (!isOperator(getChannelByName(server.getChannels(), channelName).getOpClientFd(),clientFd)){
                    response = ":" + server.getHostName() + " 482 " + server.getClients()[clientFd]->getNickName() + " " + channelName + " :You're not a channel operator\r\n";
                    break;
                }
                if (mode == "+i"){
                getChannelByName(server.getChannels(), channelName).iMode = true;
                }
                else if (mode == "-i"){
                    getChannelByName(server.getChannels(), channelName).iMode = false;
                }
                else if (mode == "+t"){
                    getChannelByName(server.getChannels(), channelName).tMode = true;
                }
                else if (mode == "-t"){
                    getChannelByName(server.getChannels(), channelName).tMode = false;
                }
                else if (mode == "+k"){
                    getChannelByName(server.getChannels(), channelName).kMode = true;
                    getChannelByName(server.getChannels(), channelName).password = extractChannelPassword(message);
                }
                else if (mode == "-k"){
                    getChannelByName(server.getChannels(), channelName).kMode = false;
                    getChannelByName(server.getChannels(), channelName).password.clear();
                }
                else if (mode == "+o"){
                    getChannelByName(server.getChannels(), channelName).oMode = true;
                }
                else if (mode == "-o"){
                    getChannelByName(server.getChannels(), channelName).oMode = false;
                }
                response = ":" + server.getHostName() + " MODE " + channelName + " " + mode + "\r\n";
            }
            catch(std::exception &e){
                std::cout << e.what() << std::endl;
                response = ":" + server.getHostName() + " 502 " + server.getClients()[clientFd]->getNickName() + " :You are now online\r\n";
            }
            break;
        case 7 :
            response = handleCapabilityNegotiation(message);
            break;
        case 8 : // Command Join
            parseChannelName(message, channelName);
            if (!doesChannelExist(server.getChannels(), channelName)){
                 std::cout << Cyan << "###" << server.getClients()[clientFd]->getNickName() << " created :" << channelName << "###" << Reset << std::endl;
                server.getChannels().push_back(Channel(channelName, *server.getClients()[clientFd]));
                // Search the channel end atribute the op to the client who created the channel
                for(std::vector<Channel>::iterator it = server.getChannels().begin(); it != server.getChannels().end(); ++it){
                    if (it->getChannelName() == channelName){
                        it->getOpClientFd().push_back(clientFd);
                    }
                }
            }
            if (getChannelByName(server.getChannels(), channelName).iMode == false &&
            getChannelByName(server.getChannels(), channelName).password.empty()){
                for(it = server.getChannels().begin(); it != server.getChannels().end();++it){
	    	    	if ((*it).getChannelName() == channelName){
	    	    		(*it).getInvitedClients().insert(std::make_pair<std::string, Client>(server.getClients()[clientFd]->getNickName(), *server.getClients()[clientFd]));
	    	    	}
	    	    }
                response2 = ":" + server.getClients()[clientFd]->getNickName() + "!~" + server.getClients()[clientFd]->getUserName() + "@" + server.getHostName() + " JOIN " + channelName + "\r\n";
                response = ":" + server.getHostName() + " 332 " + server.getClients()[clientFd]->getNickName() + " " + channelName + getChannelByName(server.getChannels(), channelName).topic + "\r\n";
                sendStatus = send (clientFd, response2.c_str(), response2.length(), 0);
                if (sendStatus == -1){
                    std::cout << "error sending nick" << std::endl;
                }
                else if (sendStatus > 0){
	    	        std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
                }
                std::cout << Cyan << "###" << server.getClients()[clientFd]->getNickName() << " joined :" << channelName << "###" << Reset << std::endl;
            }
            else{
                response = ":" + server.getHostName() + " 473 " + server.getClients()[clientFd]->getNickName() + " " + channelName + " :Cannot join channel without invitation\r\n";
            }
            break;
        case 9 : // Command Part
        //send this message to all client after deleting this client of the channel
            parseChannelName(message, channelName);
            nickName = server.getClients()[clientFd]->getNickName();
            response = ":" + nickName + " PART " + channelName + "\r\n";
            for (it = server.getChannels().begin();it != server.getChannels().end(); ++it){
                if ((*it).getChannelName() == channelName){
                    // delete the fd from the op
                    if (isOperator((*it).getOpClientFd(), clientFd)){
						for (std::vector<int>::iterator iter = it->getOpClientFd().begin(); iter != it->getOpClientFd().end(); ++iter) {
                            if (*iter == clientFd) {
                              it->getOpClientFd().erase(iter);
                              break;  // Break out of the loop after erasing the element
                            }
                        }
					}
                    (*it).getInvitedClients().erase(nickName);
                    for(iter = (*it).getInvitedClients().begin(); iter != (*it).getInvitedClients().end();++iter){
                        int sendStatus = send(iter->second.getSocketFd(),response.c_str(),response.length(), 0);
                        if (sendStatus <= 0){
                            std::cout << Red << "Failed to send message" << Reset << std::endl;
                        }
                        else if (sendStatus > 0){
                            std::cout << Cyan<< server.getClients()[clientFd]->getNickName() << " from channel -> " << channelName << " sended: " << messageContent << "to " << iter->second.getNickName() << Reset << std::endl;
                        }
                    }
                    if ((*it).getInvitedClients().empty()) {
                        it = server.getChannels().erase(it);
                        --it;
                    }
					break;
                }
            }
			if (getChannelByName(server.getChannels(), channelName).getOpClientFd().empty()){
				getChannelByName(server.getChannels(), channelName).getOpClientFd().push_back(getChannelByName(server.getChannels(), channelName).getInvitedClients().begin()->second.getSocketFd());
			}
            response = ":" + nickName + "!" + nickName + "@" + server.getHostName() + " Part " + channelName + "\r\n";
            break;
        case 10 : // Command PRIVM
            parseMessage(message, channelName, messageContent);
            for (it = server.getChannels().begin();it != server.getChannels().end(); ++it){
                if ((*it).getChannelName() == channelName){
                    if (isInChannel(clientFd, (*it))){
                        for(iter = (*it).getInvitedClients().begin(); iter != (*it).getInvitedClients().end();++iter){
                            if (iter->second.getNickName() == server.getClients()[clientFd]->getNickName()){
                                //skip
                            }
                            else{
	    				        response = ":" + server.getClients()[clientFd]->getNickName() + "!~" + server.getClients()[clientFd]->getUserName() + " PRIVMSG " + channelName + " :" + messageContent + "\r\n";
                                int sendStatus = send(iter->second.getSocketFd(),response.c_str(),response.length(), 0);
                                if (sendStatus <= 0){
                                    std::cout << Red << "Failed to send message" << Reset << std::endl;
                                }
                                else if (sendStatus > 0){
                                    std::cout << Cyan<< server.getClients()[clientFd]->getNickName() << " from channel -> " << channelName << " sended: " << messageContent << "to " << iter->second.getNickName() << Reset << std::endl;
                                }
					            if(iter->second.getSocketFd() < 0){
						            std::cerr << "socket does not exit" << std::endl;
	    				        }
                            }
                        }
                    }
                }
            }
            response = ":" + server.getHostName() + " NOTICE " + channelName + " :Message sended succefully!\r\n";
            break;
        case 11 :
            response = ":QUIT :leaving\r\n";
            for (std::map<int, Client*>::iterator it = server.getClients().begin(); it != server.getClients().end(); ++it){
                if (it->second->getNickName() == nickName){
                   server.getClients().erase(it);
                }
            }
            break;
        case 12 : // Command Kick / ps debugg if he tries to kick himself
            parseKickMessage(message, channelName ,nickName);
            response = ":" + nickName + "!" + nickName + "@" + server.getHostName() + " Part " + channelName + "\r\n";
            for (it = server.getChannels().begin();it != server.getChannels().end(); ++it){
                if ((*it).getChannelName() == channelName){
					if (isOperator((*it).getOpClientFd() , clientFd)){
						(*it).getInvitedClients().erase(nickName);
                        //isInChannel(getChannelByName(server.getChannels(), channelName).invitedClients[nickName].socketFd, getChannelByName(server.getChannels(), channelName));
                    	for(iter = (*it).getInvitedClients().begin(); iter != (*it).getInvitedClients().end();++iter){
                            if (iter->second.getSocketFd() != clientFd){
                                int sendStatus = send(iter->second.getSocketFd(),response.c_str(),response.length(), 0);
                    	        if (sendStatus <= 0){
                    	            std::cout << Red << "Failed to send message" << Reset << std::endl;
                    	        }
                    	        else if (sendStatus > 0){
							    	response = ":" + nickName + " PART " + channelName + "\r\n";
                    	            std::cout << Cyan<< server.getClients()[clientFd]->getNickName() << " from channel -> " << channelName << " sended: " << messageContent << "to " << iter->second.getNickName() << Reset << std::endl;
                    	        }
                            }
                    	}
                        //delete channel if there is no one
                    	if ((*it).getInvitedClients().empty()) {
                    	    it = server.getChannels().erase(it);
                    	    --it;
                    	}
					}
					else{
						response = NOTOPERATOR(server.getHostName(), channelName);
					}
                }
            }
            break;
        case 13 : // Command Invite
            try{
                //std::cout << channelName << std::endl;
                parseChannelName(message, channelName);
                if (getChannelByName(server.getChannels(),channelName).iMode == true){
                    getChannelByName(server.getChannels(),channelName).invitedClientsToChannel.push_back(getClientByName(server.getClients(),extractInvitedClient(message)).getSocketFd());
                    response = "invited";
                }
                else{
                    response = ":NOTICE " + server.getClients()[clientFd]->getNickName() + " :Error 518: You need to set mode +i to invite someone to the channel";
                }
            }
            catch(std::exception &e){
                std::cout << e.what() << std::endl;
                response = "cant invite";
            }
            break;
        case 14 : // Command Topic
            try{
                parseChannelName(message, channelName);
                if (isOperator(getChannelByName(server.getChannels(), channelName).getOpClientFd(), clientFd)){
                    getChannelByName(server.getChannels(), channelName).topic = extractTopic(message);
                    response = ":" + server.getHostName() + " 332 " + server.getClients()[clientFd]->getNickName() + " " + channelName + getChannelByName(server.getChannels(), channelName).topic + "\r\n";
                    messageAllChannelClients(getChannelByName(server.getChannels(), channelName), clientFd, response);
                }
                else{
                    response = ":" + server.getHostName() + " 482 " + server.getHostName() + " " + channelName + " :You're not a channel operator\r\n";
;
                }
            }
            catch(std::exception &e){
                std::cout << e.what() << std::endl;
            }

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
    if (words[0] == "INVITE"){
        channelName = words[2];
    }
    else{
        channelName = words[1];
    }
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
        if (it->getChannelName() == channelName) {
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

bool CommandHandler::isInChannel(int clientFd, Channel &channel) {
    for (std::map<std::string, Client>::iterator it = channel.getInvitedClients().begin(); it != channel.getInvitedClients().end(); ++it) {
        if (it->second.getSocketFd() == clientFd) {
            return true;
        }
    }
    return false;
}

bool CommandHandler::isOperator(std::vector<int> operators, int clientFd){
	for (std::vector<int>::iterator it = operators.begin(); it != operators.end(); ++it){
		if (*it == clientFd){
			return true;
		}
	}
	return false;
}

std::string CommandHandler::extractMode(std::string &message, std::string &channelName){
     // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        words.push_back(word);
    }
    for (size_t i = 0; i < words.size(); i++){
        if (words[i] == "MODE"){
            channelName = words[i + 1];
            return words[i + 2];
        }
    }
    return NULL;
}

std::string CommandHandler::extractChannelPassword(std:: string &message){
     // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        words.push_back(word);
    }
    for (size_t i = 0; i < words.size(); i++){
        if (words[i] == "MODE"){
            return words[i + 3];
        }
    }
    return NULL;
}

Channel& CommandHandler::getChannelByName(std::vector<Channel> &channels, std::string channelName){
    for (std::vector<Channel>::iterator it = channels.begin();it != channels.end(); ++it){
        if (it->getChannelName() == channelName){
            return *it;
        }
    }
    throw std::runtime_error("No Channel found");
}
Client& CommandHandler::getClientByName(std::map<int, Client*> &clients, std::string clientName){
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it){
        if (it->second->getNickName() == clientName){
            return *it->second;
        }
    }
    throw std::runtime_error("No Client found");
}

std::string CommandHandler::extractInvitedClient(std:: string &message){
      // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        words.push_back(word);
    }
    for (size_t i = 0; i < words.size(); i++){
        if (words[i] == "INVITE"){
            return words[i + 1];
        }
    }
    return NULL;
}

std::string CommandHandler::extractTopic(std::string message){
          // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    std::string topic;
    while (iss >> word) {
        words.push_back(word);
    }
    for (size_t i = 2; i < words.size(); i++){
        topic += " ";
        topic += words[i];
    }
    return topic;
}

void CommandHandler::messageAllChannelClients(Channel channel, int client, std::string message){
    for (std::map<std::string, Client>::iterator it = channel.getInvitedClients().begin(); it != channel.getInvitedClients().end(); ++it){
        if (it->second.getSocketFd() != client){
            send(it->second.getSocketFd(), message.c_str(), message.length(), 0);
        }
    }
}