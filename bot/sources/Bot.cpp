#include "../classes/Bot.hpp"

bool Bot::running = false;

Bot::Bot(std::string botName, std::string serverIp, int port, std::string password) :
    botName(botName), serverIp(serverIp), port(port), password(password) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGQUIT, signalHandler);
    startingCount = 0;
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
    startingCount = time(NULL);
    time_t lastActivityTime = startingCount;  // Initialize with the starting time

    while (running) {
        int pollResult = poll(fds, 1, 5000);  // 5-second timeout
        if (pollResult == -1) {
            send(socketFd, quit.c_str(), quit.length(), 0);
            throw std::runtime_error("Failed to poll.");
        } else if (pollResult == 0) {
            // Timeout occurred, handle it as needed
            time_t currentTime = time(NULL);
            if (currentTime - lastActivityTime >= 60) {
                // No activity for 30 seconds, send a message to the server
                std::string idleMessage = "LIST -YES\r\n";;
                ssize_t bytesSent = send(socketFd, idleMessage.c_str(), idleMessage.length(), 0);
                if (bytesSent == -1) {
                    throw std::runtime_error("Failed to send idle message.");
                } else {
                    std::cout << Yellow << "Bot sent idle message." << Reset << std::endl;
                }
                lastActivityTime = currentTime;  // Update the last activity time
            }
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
                lastActivityTime = time(NULL);  // Update the last activity time
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
    std::string auth = "CAP LS\nPASS " + password + "\nNICK " + botName + "\nUSER " + botName + " " + botName + " " + botName + " " + botName + "\r\n";
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
    std::string msg;
    if (message.find("PRIVMSG ") != std::string::npos){
        getMessageInfo(message, clientName, channelName, content);
    }
    if (game.find(channelName) != game.end() && game[channelName] != NULL && game[channelName]->getRunning() && isPlaying(clientName, game[channelName]->getPlayers())){
    time_t currentTime = time(NULL);
	double duration = difftime(currentTime, game[channelName]->getStart());
        std::string result;
        std::cout << game[channelName]->getNumberOfQuestions() << std::endl;
		updatedMessage = game[channelName]->update(message, clientName);
		if (duration > 30){
			extractTopicAndQuestion(updatedMessage, game[channelName]->getCurrentTopic(), game[channelName]->getCurrentQuestion());
        	result = CUSTUM_MESSAGE(channelName, "<Too late <TIME OUT> for this question!");
            game[channelName]->decrementNumberOfQuestions();
        	send(socketFd, result.c_str(), result.length(), 0);
            if (game[channelName]->getNumberOfQuestions() != 8){
                game[channelName]->setStart(time(NULL));
                std::string result;
                extractTopicAndQuestion(game[channelName]->getRandomQuestion(), game[channelName]->getCurrentTopic(), game[channelName]->getCurrentQuestion());
                result = "PRIVMSG " + channelName + " :" + game[channelName]->getCurrentTopic() + " => " + game[channelName]->getCurrentQuestion() + "\r\n";
                send(socketFd, result.c_str(), result.length(), 0);
			    return;
            }
		}
		if (game[channelName]->getNumberOfQuestions() != 8 && updatedMessage == "BADANSWER!#"){
			result = CUSTUM_MESSAGE(channelName, "<WRONG AWNSER>");
        	send(socketFd, result.c_str(), result.length(), 0);
		}
		else{
			if (game[channelName]->getNumberOfQuestions() == 8){
				// Display the winner by calling a function that find the best
                std::string winner = whoWonGame(game[channelName]->getPlayers());
                if (winner == "NO_WINNER"){
				    victoryMessage = CUSTUM_MESSAGE(channelName, "There is no winner you <all == NULL>!");
                }
                else if (winner == "DRAW"){
                    victoryMessage = CUSTUM_MESSAGE(channelName, "There is a draw and i wont tell names ...!");
                }
                else{
                    victoryMessage = CUSTUM_MESSAGE(channelName, winner) + " Won the game!";
                }
        	    send(socketFd, victoryMessage.c_str(), victoryMessage.length(), 0);
				game[channelName]->setRunning(false);
				std::map<std::string, Game*>::iterator it = game.find(channelName);
				if (it != game.end()){
					delete it->second;
					game.erase(it);
				}
				return;
        	}
			extractTopicAndQuestion(updatedMessage, game[channelName]->getCurrentTopic(), game[channelName]->getCurrentQuestion());
        	result = "PRIVMSG " + channelName + " :" + game[channelName]->getCurrentTopic() + " => " + game[channelName]->getCurrentQuestion() + "\r\n";
        	send(socketFd, result.c_str(), result.length(), 0);
		}
    }
    else{
        if (message.find("001 ") != std::string::npos && message.find("\r\n")) {
            response = "LIST -YES\r\n";
        } else if (message.find("LIST:") != std::string::npos && message.find("\r\n") != std::string::npos) {
            joinEveryChannel(message);
        }
        else if (message.find("PRIVMSG ") != std::string::npos && message.find("play") != std::string::npos && message.find("\r\n") != std::string::npos){
            if (createGame(channelName, clientName)){
                msg = CUSTUM_MESSAGE(channelName, "Game Created, you are now able to join the game before starts");
                send(socketFd, msg.c_str(), msg.length(), 0);
            }
        }
         else if (message.find("PRIVMSG ") != std::string::npos && message.find("join") != std::string::npos && message.find("\r\n") != std::string::npos){
            if (joinPlayer(channelName, clientName)){
                msg = CUSTUM_MESSAGE(channelName, clientName + " has joined succefully the game");
                send(socketFd, msg.c_str(), msg.length(), 0);
            }
        }
        else if (message.find("PRIVMSG ") != std::string::npos && message.find("start") != std::string::npos && message.find("\r\n") != std::string::npos){
            if (!game[channelName]->getPlayers().empty()){
                startGame(channelName);
            }
            else{
                msg = CUSTUM_MESSAGE(channelName, clientName + " You cant start the game without players <at least one>");
                send(socketFd, msg.c_str(), msg.length(), 0);
            }
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
    std::string rulesForGame = "Hi, I am " + botName + ", i was created to entretain channel by playing quiz games.Here are some rules <Write play to create a game>  <Write join if you want to participate in the game>  <Write start when all member are ready to start the Quiz>  <To aweser the questions you need to type * followed by the aweser>";
    while (iss >> channelName) {
        channels.push_back(channelName);
    }
    for (size_t i = 1; i < channels.size(); i++) {
        if (!game[channels[i]]){
            request = "JOIN " + channels[i] + "\r\n";
            send(socketFd, request.c_str(), request.length(), 0);
            sleep(1);
            std::string rules = CUSTUM_MESSAGE(channels[i], rulesForGame);
            send(socketFd, rules.c_str(), rules.length(), 0);
            sleep(1);
        }
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

bool Bot::createGame(std::string channelName, std::string clientName) {
    (void)clientName;
    std::string response;
    if (game.find(channelName) != game.end() && game[channelName] != NULL) {
        if (game[channelName]->getRunning()){
            std::cout << Red << "Game already began" << Reset << std::endl;
            response = "PRIVMSG " + channelName + " :The game have already started, wait till it finish so you can create another one!\r\n";
            send(socketFd, response.c_str(), response.length(), 0);
            return false;
        }
        else{
            std::cout << "Game already exists" << std::endl;
            response = "PRIVMSG " + channelName + " :The game was already created, type <join> to be able to play!\r\n";
            send(socketFd, response.c_str(), response.length(), 0);
            return false;
        }
    } else {
        game[channelName] = new Game("game.txt", channelName);
    }
    return true;
}


bool Bot::joinPlayer(std::string channelName, std::string clientName){
    std::string response;
    if (game.find(channelName) != game.end() && game[channelName] != NULL) {
        if (game[channelName]->getRunning()){
            std::cout << Red << "Game already began" << Reset << std::endl;
            response = "PRIVMSG " + channelName + " :You are already late to <join> the game that already began!\r\n";
            send(socketFd, response.c_str(), response.length(), 0);
            return false;
        }
        else{
            game[channelName]->addPlayer(clientName);
            return true;
        }
    }
    else {
        std::cout << "cant join the game that is not created" << std::endl;
        response = "PRIVMSG " + channelName + " :You cant <join> a game that was not created yet!\r\n";
        send(socketFd, response.c_str(), response.length(), 0);
    }
    return false;
}

void Bot::startGame(std::string channelName){
    std::string result;
    if (game.find(channelName) != game.end() && game[channelName] != NULL)
    {
        extractTopicAndQuestion(game[channelName]->run(), game[channelName]->getCurrentTopic(), game[channelName]->getCurrentQuestion());
        result = "PRIVMSG " + channelName + " :" + game[channelName]->getCurrentTopic() + " => " + game[channelName]->getCurrentQuestion() + "\r\n";
    }
    else{
        result = "PRIVMSG " + channelName + " :You cant start a game that was not yet created\r\n";
        std::cout<<Red<<"The game doesnot exist"<<Reset<<std::endl;
    }
    send(socketFd, result.c_str(), result.length(), 0);
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
	int score = -1;
	std::string winner;
	for (std::map<std::string, int>::iterator it = players.begin(); it != players.end(); it++){
		if (it->second > score){
			score = it->second;
			winner = it ->first;
		}
	}
    if (players[winner] == 0){
        return "NO_WINNER";
    }
    for (std::map<std::string, int>::iterator it = players.begin(); it != players.end(); it++){
		if (it->first != winner){
            if (it->second == players[winner]){
                return "DRAW";
            }
		}
	}
	return winner;
}

bool Bot::isPlaying(std::string clientName, std::map<std::string, int> players){
    for (std::map<std::string, int>::iterator it = players.begin(); it != players.end(); ++it){
        if (it->first == clientName){
            return true;
        }
    }
    return false;
}

time_t Bot::staringCount(){
    return this->startingCount;
}

void Bot::setStartingCount(time_t time){
    startingCount = time;
}
