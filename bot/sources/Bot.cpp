#include "../classes/Bot.hpp"

bool Bot::running = false;

Bot::Bot(std::string botName, std::string serverIp, int port, std::string password) :
    botName(botName), serverIp(serverIp), port(port), password(password) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGQUIT, signalHandler);
    std::cout << "Bot Created" << std::endl;
}

Bot::~Bot() {
    std::cout << Red << "Bot Destructed" << std::endl;
}

void Bot::run() {
    connectToServer();
    authenticate();
    running = true;
    char buffer[1024];
    std::string response;
    std::string quit = "QUIT :leaving\r\n";

    struct pollfd fds[1];
    fds[0].fd = socketFd;
    fds[0].events = POLLIN;

    while (running) {
        int pollResult = poll(fds, 1, 5000);  // 5-second timeout
        if (pollResult == -1) {
            send(socketFd, quit.c_str(), quit.length(), 0);
            throw std::runtime_error("Failed to poll.");
        } else if (pollResult == 0) {
            // Timeout occurred, handle it as needed
            continue;
        } else {
            if (fds[0].revents & POLLIN) {
                // Socket is ready for reading

                bzero(buffer, sizeof(buffer));
                int recvBytes = recv(socketFd, buffer, sizeof(buffer), 0);
                if (recvBytes < 0) {
                    throw std::runtime_error("Failed to receive data from client.");
                } else {
                    std::cout << Green << "Message Received: " << Reset << buffer << std::endl;
                }

                std::string message(buffer);
                handleMessage(message, response);
                if (!response.empty()) {
                    ssize_t bytesSent = send(socketFd, response.c_str(), response.length(), 0);
                    if (bytesSent == -1) {
                        throw std::runtime_error("Failed to send authentication message.");
                    } else {
                        std::cout << Blue << "Bot sent: " << Reset << response << std::endl;
                    }
                }
                response.clear();
            }
        }
    }

    send(socketFd, quit.c_str(), quit.length(), 0);
}

void Bot::connectToServer() {
    createSocket();
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(port);
    socketAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());

    int connectResult = connect(socketFd, (sockaddr*)&socketAddr, sizeof(socketAddr));
    if (connectResult == -1 && errno == EINPROGRESS) {
        // Connection in progress

        struct pollfd fds[1];
        fds[0].fd = socketFd;
        fds[0].events = POLLOUT;

        int pollResult = poll(fds, 1, 5000);  // 5-second timeout
        if (pollResult == -1) {
            close(socketFd);
            throw std::runtime_error("Failed to poll.");
        } else if (pollResult == 0) {
            close(socketFd);
            throw std::runtime_error("Connection timeout.");
        } else {
            if (fds[0].revents & POLLOUT) {
                // Socket is ready for writing, check if the connection was successful

                int error;
                socklen_t len = sizeof(error);
                if (getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &error, &len) == -1 || error != 0) {
                    close(socketFd);
                    throw std::runtime_error("Failed to connect.");
                }
            } else {
                close(socketFd);
                throw std::runtime_error("Connection failed.");
            }
        }
    } else if (connectResult == -1) {
        close(socketFd);
        throw std::runtime_error("Failed to connect.");
    }
    std::cout << "Connected" << std::endl;
}

void Bot::createSocket() {
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
    if (fcntl(socketFd, F_SETFL, O_NONBLOCK) == -1) {
        std::cout << Red << "Error setting socket to non-blocking mode." << Reset << std::endl;
    }
}

void Bot::authenticate() {
    std::string auth = "CAP LS\nPASS 123\nNICK BOT\nUSER BOT BOT BOT BOT\r\n";
    ssize_t bytesSent = send(socketFd, auth.c_str(), auth.length(), 0);

    if (bytesSent == -1) {
        throw std::runtime_error("Failed to send authentication message.");
    } else {
        std::cout << "Authentication message sent" << std::endl;
    }
}

void Bot::handleMessage(std::string& message, std::string& response) {
    std::string clientName;
    std::string channelName;
    std::string content;
	std::string victoryMessage;
	std::string updatedMessage;
    if (message.find("PRIVMSG ") != std::string::npos){
        getMessageInfo(message, clientName, channelName, content);
    }
	std::cout << "The Client name is" << clientName << std::endl;
    if (game.find(channelName) != game.end() && game[channelName] != NULL && game[channelName]->running){
        // std::string update = game[channelName]->update(message);
        std::string result;
		updatedMessage = game[channelName]->update(message, clientName);
		if (updatedMessage == "BADANSWER!#"){
			result = CUSTUM_MESSAGE(channelName, "WRONG AWSER");
        	send(socketFd, result.c_str(), result.length(), 0);
		}
		else{
			if (game[channelName]->getNumberOfQuestions() == 8){
				// Display the winner by calling a function that find the best
				victoryMessage = CUSTUM_MESSAGE(channelName, whoWonGame(game[channelName]->getPlayers()) + " Won the game!");
        	    send(socketFd, victoryMessage.c_str(), victoryMessage.length(), 0);
				// delete game[channelName];
				return;
        	}
			extractTopicAndQuestion(updatedMessage, game[channelName]->currentTopic, game[channelName]->currentQuestion);
        	result = "PRIVMSG " + channelName + " :" + game[channelName]->currentTopic + " => " + game[channelName]->currentQuestion + "\r\n";
        	send(socketFd, result.c_str(), result.length(), 0);
        	
		}

    }
    else{
        if (message.find("001 ") != std::string::npos && message.find("\r\n")) {
            response = "LIST -YES\r\n";
        } else if (message.find("LIST:") != std::string::npos && message.find("\r\n") != std::string::npos) {
            joinEveryChannel(message);
            std::string rules = RULES("#a");
            send(socketFd, rules.c_str(), rules.length(), 0);
        }
        else if (message.find("PRIVMSG ") != std::string::npos && message.find("play") != std::string::npos && message.find("\r\n") != std::string::npos){
            createGame(channelName, clientName);
        }
         else if (message.find("PRIVMSG ") != std::string::npos && message.find("join") != std::string::npos && message.find("\r\n") != std::string::npos){
            joinPlayer(channelName, clientName);
        }
        else if (message.find("PRIVMSG ") != std::string::npos && message.find("start") != std::string::npos && message.find("\r\n") != std::string::npos){
            startGame(channelName);
        }
    }
    message.clear();
}

void Bot::joinEveryChannel(std::string message) {
    std::istringstream iss(message);
    std::string channelName;
    std::vector<std::string> channels;
    std::string topic;
    std::string request;
    while (iss >> channelName) {
        channels.push_back(channelName);
    }
    for (size_t i = 1; i < channels.size(); i++) {
        request = "JOIN " + channels[i] + "\r\n";
        send(socketFd, request.c_str(), request.length(), 0);
        sleep(3);
    }
}

void Bot::signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGQUIT) {
        std::cout << Green << "Thanks for using our bot!" << Reset << std::endl;
        std::cout << Red << "Bot Closed" << Reset << std::endl;
        running = false;
    }
}

void Bot::getMessageInfo(std::string message, std::string &clientName, std::string &channelName, std::string &content){
    std::istringstream iss(message);
    std::string word;
    std::vector<std::string> words;
    std::string topic;
    while (iss >> word) {
        words.push_back(word);
    }
    clientName = words[0];
    channelName = words[2];
    content = words[3];
}

void Bot::createGame(std::string channelName, std::string clientName) {
    (void)clientName;
    if (game.find(channelName) != game.end() && game[channelName] != NULL) {
        if (game[channelName]->running){
            std::cout << Red << "Game already began" << Reset << std::endl;
        }
        else{
            std::cout << "Game already exists" << std::endl;
        }
    } else {
        game[channelName] = new Game("game.txt", channelName);
    }
}


void Bot::joinPlayer(std::string channelName, std::string clientName){
    if (game.find(channelName) != game.end() && game[channelName] != NULL) {
        if (game[channelName]->running){
            std::cout << Red << "Game already began" << Reset << std::endl;
        }
        else{
            game[channelName]->addPlayer(clientName);
        }
    }
    else {
        std::cout << "cant join the game that is not created" << std::endl;
    }
}

void Bot::startGame(std::string channelName){
    if (game.find(channelName) != game.end() && game[channelName] != NULL)
    {
        std::string result;
        extractTopicAndQuestion(game[channelName]->run(), game[channelName]->currentTopic, game[channelName]->currentQuestion);
        result = "PRIVMSG " + channelName + " :" + game[channelName]->currentTopic + " => " + game[channelName]->currentQuestion + "\r\n";
        send(socketFd, result.c_str(), result.length(), 0);
    }
    else{
        std::cout<<Red<<"The game doesnot exist"<<Reset<<std::endl;
    }
}

void Bot::extractTopicAndQuestion(const std::string& input, std::string& topic, std::string& question) {
    std::size_t pos = input.find_first_of("\n:");
    if (pos != std::string::npos) {
        std::size_t endPos = input.find_first_of("\n:", pos + 1);
        if (endPos != std::string::npos) {
            topic = input.substr(pos + 1, endPos - pos - 1);
            question = input.substr(endPos + 1);
            if (question.substr(0, 10) == "Question: ") {
                question = question.substr(10);
            }
        }
    }
    // Trim leading space from topic
    if (!topic.empty() && topic[0] == ' ') {
        topic = topic.substr(1);
    }
    // Trim trailing newline character from question
    if (!question.empty() && question[question.length() - 1] == '\n') {
        question = question.substr(0, question.length() - 1);
    }
}

std::string Bot::whoWonGame(std::map<std::string, int> players){
	int score = 0;
	std::string winner;
	for (std::map<std::string, int>::iterator it = players.begin(); it != players.end(); it++){
		if (it->second > score){
			score = it->second;
			winner = it ->first;
		}
	}
	return winner;
}