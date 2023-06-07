#include "../classes/Server.hpp"

Server::Server(unsigned int port, std::string password) :
    port(port), password(password), socketFd(0) {
    std::cout << "Server Constructor" << std::endl;
}

Server::~Server() {
}

void Server::run() {
    std::cout << Green << "Server Running ..." << Reset<< std::endl;
    createSocket();
}

void Server::createSocket() {
    std::cout << Yellow << "Creating Server Socket" << Reset << std::endl;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        throw std::runtime_error("Failed to create socket.");
    }
    int reuseAddr = 1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseAddr, sizeof(reuseAddr)) == -1) {
        close(socketFd);
        throw std::runtime_error("Failed to set SO_REUSEADDR option.");
    }
    //testing to see if changes
   if(fcntl(socketFd, F_SETFL, O_NONBLOCK)){
        std::cout << "error fcntl " << std::endl;
   }
    bindSocket();
    listening();
}

void Server::bindSocket() {
    try {
        struct sockaddr_in actualAddr;
        socklen_t actualAddrLen = sizeof(actualAddr);
        // Initialize Struct socketAddr
        socketAddr.sin_family = AF_INET;
        socketAddr.sin_port = htons(port);
        socketAddr.sin_addr.s_addr = htons(INADDR_ANY);
        // Bind the Socket
        int bindStatus = bind(socketFd, (struct sockaddr*)&socketAddr, sizeof(socketAddr));
        if (bindStatus == -1) {
            throw std::runtime_error("Failed to bind socket.");
        }
        // Get Socket Name
        if (getsockname(socketFd, (struct sockaddr*)&actualAddr, &actualAddrLen) == -1) {
            throw std::runtime_error("Failed to get socket name.");
        }
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

void Server::listening() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    listen(socketFd, 5);
    std::cout << Green << "Listening Clients Connections ..." << Reset << std::endl;
    // Create a vector of pollfd structures to hold file descriptors and events
    std::vector<pollfd> pollFds;
    pollfd serverPollFd;
    serverPollFd.fd = socketFd;
    serverPollFd.events = POLLIN;
    serverPollFd.revents = 0;
    pollFds.push_back(serverPollFd);
    while (true) {
        // Wait for activity on any of the monitored file descriptors
        int pollResult = poll(pollFds.data(), pollFds.size(), -1);
        if (pollResult == -1) {
            throw std::runtime_error("Error in poll system call.");
        }
        // Check if there is a new client connection   
        if (pollResult > 0){
            if (pollFds[0].revents & (POLLIN)) {
                int clientSocket = accept(socketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
                if (clientSocket == -1) {
                    throw std::runtime_error("Failed to accept a client connection.");
                }
                // Add the new client socket to the vector of pollfd structures
                pollfd clientPollFd;
                clientPollFd.fd = clientSocket;
                clientPollFd.events = POLLIN;
                clientPollFd.revents = 0;
                pollFds.push_back(clientPollFd);
            }
        }
        handleCommunication(pollFds);
    }
}

void Server::handleCommunication(std::vector<pollfd>& pollFds) {
    // Iterate through all the file descriptors in the vector
    for (int i = pollFds.size() - 1; i >= 1; --i) {
        if (pollFds[i].revents & POLLIN) {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            // Receive Data from Client
            int recvBytes = recv(pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);
            if (recvBytes <= 0) {
                throw std::runtime_error("Failed to receive data from client.");
            }
            else if (recvBytes == 0) {
                // Client closed the connection
                close(pollFds[i].fd);
                pollFds.erase(pollFds.begin() + i);
                // --i;
                continue;
            }
            else {
                buffer[recvBytes] = '\0';
                std::string message(buffer);
                handleCommands(message, pollFds[i]);
            }
        }
    }
}

std::string Server::handleCapabilityNegotiation(const std::string& message) {
    // Check if the received message is CAP LS
    if (message.substr(0, 8) != "CAP LS\r\n")
        return "Unknown command.";
    // Prepare the list of supported capabilities
    std::string capabilities = "CAP LIST";
    // Construct the response
    std::string response = "CAP * LS :" + capabilities + "\r\n";
    return response;
}

// Function to parse the nickname message
bool Server::parseNicknameMessage(const std::string& message, std::string& nickname, std::string& username)
{
    // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        words.push_back(word);
    }
    // Extract the nickname and username
    nickname = words[3];
    username = words[5];
    return true;
}

void Server::handleCommands(std::string message, pollfd& pollFds){
    std::string response;
    std::cout << Green << "Received Data From Client: " << Reset << message << std::endl;
    if (message.find("NICK ") != std::string::npos && message.find("USER ") != std::string::npos) {
        // Handle the user information and connection establishment
        std::string nickName;
        std::string userName;
        parseNicknameMessage(message, nickName, userName);
		std::cout << "parsed nickname : " << nickName << userName << std::endl;
        Client *client = new Client(nickName, pollFds.fd);
        clients.insert(std::pair<int, Client*>(pollFds.fd, client));
        std::cout << clients.begin()->first << std::endl;
        // Send a welcome message to the client
        std::string welcomeMessage = ":localhost 001 " + clients[pollFds.fd]->nickname + ": " +  readFile("wel.txt")  + client->nickname + readFile("come.txt") + "\r\n";
		std::cout << welcomeMessage << std::endl;
		std::cout << "pollfd = " << pollFds.fd << std::endl;
		std::cout << "nickname: " << clients[pollFds.fd]->nickname << std::endl;
        int sendStatus = send(pollFds.fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
        std::cout << Blue << "Server Sended Response with: " << Reset << welcomeMessage << std::endl;
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("WHOIS ") != std::string::npos){
       response = ":localhost 311 " + clients[pollFds.fd]->nickname + " localhost " + clients[pollFds.fd]->nickname + " *\r\n";
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("CAP END\r\n") != std::string::npos){
        response = ":localhost CAP * ACK :END\r\n";
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if(message.find("PING ") != std::string::npos){
        // Handle the ping message
        response = "PONG localhost\r\n";
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("MODE ") != std::string::npos){
        response = ":localhost 221 " + clients[pollFds.fd]->nickname + " -I\r\n";
        std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("CAP ") != std::string::npos) {
        // Handle capability negotiation commands
        response = handleCapabilityNegotiation(message);
        std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("JOIN ") != std::string::npos) {
        for(std::map<int, Client*>::iterator it = clients.begin(); it != clients.end();++it){
            // response = ":server.example.com 464 yourNickname :NickName already used please type <NICK new_nickname>\r\n";
            // int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
            // std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
            // if (sendStatus == -1) {
            //     throw std::runtime_error("Failed to send data to client.");
            // }
            // return;
        }
        std::string channelName;
        parseChannelName(message, channelName);
        // Handle join channel commands
        for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end();++it){
            if ((*it).channelName == channelName){
                std::cout << Cyan << clients[pollFds.fd]->nickname << " joined channel :" << channelName << Reset << std::endl;
                (*it).invitedClients.insert(std::make_pair<std::string, Client>(clients[pollFds.fd]->nickname, *clients[pollFds.fd]));
                return;
            }
        }
        std::cout << Yellow << clients[pollFds.fd]->nickname << " is creating a channel ..." << Reset << std::endl;
        channels.push_back(Channel(channelName, *clients[pollFds.fd]));
        for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end();++it){
			if ((*it).channelName == channelName){
				(*it).invitedClients.insert(std::make_pair<std::string, Client>(clients[pollFds.fd]->nickname, *clients[pollFds.fd]));
			}
		}
        // Channel la(channelName, clients[pollFds.fd]);
        // response = handleJoinChannel(message);
    }
    else if (message.substr(0, 4) == "PART ") {
        // Handle leave channel commands
        response = "NOTICE " + clients[pollFds.fd]->nickname + " :CLOSE\r\n";
        send(pollFds.fd,response.c_str(), response.length(), 0);
    }
	else if (message.find("NICK ") != std::string::npos){
		clients[pollFds.fd]->nickname = "another";
        response = "NICK another";
        int sendStatus = send (pollFds.fd, response.c_str(), response.length(), 0);
        if (sendStatus == -1){
            std::cout << "error sendin nick" << std::endl;
        }
        else if (sendStatus > 0){
		    std::cout << "this client name now is " << clients[pollFds.fd]->nickname << std::endl;
        }
	}
    else if (message.substr(0, 8) == "PRIVMSG "){
        // Handle private message commands
        std::string senderNickname;
        std::string messageContent;
        std::string channelName;
        std::string messageType;
		std::string test;
        parseMessage(message, channelName, messageContent);
        for (std::vector<Channel>::iterator it = channels.begin();it != channels.end(); ++it){
            if ((*it).channelName == channelName){
                for(std::map<std::string, Client>::iterator iter = (*it).invitedClients.begin(); iter != (*it).invitedClients.end();++iter){
                    if (iter->second.nickname == clients[pollFds.fd]->nickname){
                        
                    }
                    else{
					    test = "PRIVMSG " + channelName + " :" + messageContent + "\r\n";
					     int sendStatus = send(iter->second.socketFd,test.c_str(),test.length(), 0);
                         if (sendStatus <= 0){
                            std::cout << Red << "Failed to send message" << Reset << std::endl;
                         }
                         else if (sendStatus > 0){
                            std::cout << Cyan<< clients[pollFds.fd]->nickname << " from channel -> " << channelName << " sended: " << messageContent << "to " << iter->second.nickname << Reset << std::endl;
                         }
					    if(iter->second.socketFd < 0){
						std::cerr << "socket does not exit" << std::endl;
					}
                    }
                }
            }
        }
        //send notification of sended message
        //need review not working
        // std::string la = " message_sended";
        // std::string sendedResponse = "PRIVMSG " + channelName + " :" + la;
        // send(pollFds.fd, sendedResponse.c_str(), sendedResponse.length(), 0);
    }
    else {
        // Handle unrecognized commands
        response = "Unknown command.\r\n";
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
}

bool Server::parseChannelName(const std::string& message, std::string& channelName)
{
    // Split the message into words using whitespace as delimiter
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        words.push_back(word);
    }
    // Extract the nickname and username
    channelName = words[1];
    return true;
}
bool Server::parseMessage(std::string message, std::string &channelName, std::string &messageContent){
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
std::string Server::readFile(const std::string& filePath) {
    std::ifstream file(filePath.c_str());
    std::stringstream buffer;
    
    if (file) {
        buffer << file.rdbuf();
        file.close();
    }
    
    return buffer.str();
}