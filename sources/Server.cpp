#include "../classes/Server.hpp"

Server::Server(unsigned int port, std::string password) :
    port(port), password(password), socketFd(0), hostName("localhost") {
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
        //Give server new name baseon ip
        hostName = inet_ntoa(socketAddr.sin_addr);
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
                handleClientMessage(message, pollFds[i]);
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

// Function to parse the nickName message
bool Server::parseNickNameMessage(const std::string& message, std::string& nickName, std::string& username, std::string& hostName, std::string&serverHostName, std::string& realName){
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

// void Server::handleClientMessage(std::string message, pollfd& pollFds){
//     std::cout << Green << "=> Received Data From Client: " << Reset << message << std::endl;
//     std::string response;
//     CommandHandler::handleCommand(*this, message, response);
//     //send
// }

bool Server::parseChannelName(const std::string& message, std::string& channelName)
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






void Server::handleClientMessage(std::string message, pollfd& pollFds){
    std::string response;
    std::cout << Green << "=> Received Data From Client: " << Reset << message << std::endl;
    if (message.find("NICK ") != std::string::npos && message.find("USER ") != std::string::npos) {
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
        // Send a welcome message to the client
        std::string welcomeMessage = ":" + this->hostName + " 001 " + clients[pollFds.fd]->nickName + " :" +  readFile("wel.txt")  + " " + client->nickName + " " + readFile("come.txt") + "\r\n";
        int sendStatus = send(pollFds.fd, welcomeMessage.c_str(), welcomeMessage.length(), 0);
        std::cout << Blue << "=> Server Sended Response with: " << Reset << welcomeMessage << std::endl;
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("NICK ") != std::string::npos){
        //verify if is empty and if is in use
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
        //
    }
    else if (message.find("WHOIS ") != std::string::npos){
        /*The client sends a WHOIS command to the server for a specific user.

Client sends: WHOIS username
The server acknowledges the WHOIS command and starts gathering information.

Server responds: :server-name 311 your-nick target-nick target-user target-host * :target-real-name
The server provides additional information about the user, such as server, channels, and server operator status.

Server responds: :server-name 312 your-nick target-nick target-server :server-info
Server responds: :server-name 319 your-nick target-nick :@#channel1 +#channel2
If the user is an IRC operator, the server may provide additional details.

Server responds: :server-name 313 your-nick target-nick :is an IRC Operator
The server concludes the WHOIS response.

Server responds: :server-name 318 your-nick target-nick :End of WHOIS list.*/
       response = ":" + this->hostName + " 311 " + clients[pollFds.fd]->nickName + " localhost " + clients[pollFds.fd]->nickName + " *\r\n";
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("CAP END\r\n") != std::string::npos){
        response = ":localhost CAP * ACK :END\r\n";
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if(message.find("PING ") != std::string::npos){
        // Handle the ping message
        response = "PONG " + this->hostName + "\r\n";
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("MODE ") != std::string::npos){
        response = ":" + this->hostName + " 221" + clients[pollFds.fd]->nickName + " -I\r\n";
        std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("CAP ") != std::string::npos) {
        // Handle capability negotiation commands
        response = handleCapabilityNegotiation(message);
        std::cout << Blue << "=> Server Sended Response with: " << Reset << response << std::endl;
        int sendStatus = send(pollFds.fd, response.c_str(), response.length(), 0);
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
    }
    else if (message.find("JOIN ") != std::string::npos) {
        for(std::map<int, Client*>::iterator it = clients.begin(); it != clients.end();++it){
            // response = ":server.example.com 464 yournickName :nickName already used please type <NICK new_nickName>\r\n";
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
        std::cout << Cyan << "###" << clients[pollFds.fd]->nickName << " joined :" << channelName << "###" << Reset << std::endl;
        channels.push_back(Channel(channelName, *clients[pollFds.fd]));
        for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end();++it){
			if ((*it).channelName == channelName){
				(*it).invitedClients.insert(std::make_pair<std::string, Client>(clients[pollFds.fd]->nickName, *clients[pollFds.fd]));
                // repeated code
			}
		}
        // Channel la(channelName, clients[pollFds.fd]);
        std::string response2 = ":" + clients[pollFds.fd]->nickName + "!~" + clients[pollFds.fd]->userName + "@" + hostName + " JOIN " + channelName + "\r\n";
        response = ":" + this->hostName + " 332 " + clients[pollFds.fd]->nickName + " " + channelName + " :Welcome to " + channelName + "\r\n";
        int sendStatus = send (pollFds.fd, response.c_str(), response.length(), 0);
        if (sendStatus == -1){
            std::cout << "error sendin nick" << std::endl;
        }
        else if (sendStatus > 0){
		    std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
            sendStatus = send (pollFds.fd, response2.c_str(), response2.length(), 0);
        }
    }
    else if (message.find("PART ") != std::string::npos) {
        // Handle leave channel commands
        response = "PART\r\n";
       int sendStatus = send(pollFds.fd,response.c_str(), response.length(), 0);
        if (sendStatus == -1) {
            throw std::runtime_error("Failed to send data to client.");
        }
        else{
            std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
        }
    }
	else if (message.find("NICK ") != std::string::npos){
		clients[pollFds.fd]->nickName = "another";
        response = "NICK another";
        int sendStatus = send (pollFds.fd, response.c_str(), response.length(), 0);
        if (sendStatus == -1){
            std::cout << "error sendin nick" << std::endl;
        }
        else if (sendStatus > 0){
		    std::cout << Yellow<< "Client changed nickname to <" << clients[pollFds.fd]->nickName << ">" << Reset<< std::endl;
        }
	}
    else if (message.substr(0, 8) == "PRIVMSG "){
        // Handle private message commands
        std::string sendernickName;
        std::string messageContent;
        std::string channelName;
        std::string messageType;
		std::string test;
        parseMessage(message, channelName, messageContent);
        for (std::vector<Channel>::iterator it = channels.begin();it != channels.end(); ++it){
            if ((*it).channelName == channelName){
                for(std::map<std::string, Client>::iterator iter = (*it).invitedClients.begin(); iter != (*it).invitedClients.end();++iter){
                    if (iter->second.nickName == clients[pollFds.fd]->nickName){
                        
                    }
                    else{
					    test = ":" + clients[pollFds.fd]->nickName + "!~" + clients[pollFds.fd]->userName + " PRIVMSG " + channelName + " :" + messageContent + "\r\n";
					     int sendStatus = send(iter->second.socketFd,test.c_str(),test.length(), 0);
                         if (sendStatus <= 0){
                            std::cout << Red << "Failed to send message" << Reset << std::endl;
                         }
                         else if (sendStatus > 0){
                            std::cout << Cyan<< clients[pollFds.fd]->nickName << " from channel -> " << channelName << " sended: " << messageContent << "to " << iter->second.nickName << Reset << std::endl;
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
        else{
            std::cout << Blue << "Server Sended Response with: " << Reset << response << std::endl;
        }
    }
}