#include "../classes/Game.hpp"

Game::Game(std::string file, std::string channelName) : running(false), channelName(channelName), numberOfQuestions(10){
    readQuestions(file);
    std::cout << "Game created" << std::endl;
    printQuestions();
}
Game::~Game(){}

std::string Game::run(){
    running = true;
    return getRandomQuestion();
}

void Game::addPlayer(std::string player){
    players.insert(std::make_pair(player, 0));
    std::cout << player << " added" << std::endl;
}

std::string Game::update(std::string message) {
    if (message.find("PRIVMSG ") != std::string::npos && message.find(":*") != std::string::npos && message.find("\r\n") != std::string::npos) {
        std::cout << "Current Topic: " << currentTopic << std::endl;
        std::cout << "Current Question: " << currentQuestion << std::endl;
        message = caseToLower(message);
        if (question.count(currentTopic) > 0 && question[currentTopic].count(currentQuestion) > 0) {
            std::string answer = caseToLower(question[currentTopic][currentQuestion]);
            std::cout << "Answer: " << answer << std::endl;
            if (message.find(":*" + answer) != std::string::npos) {
                std::cout << "CORRECT ANSWER!" << std::endl;
                ++players["gameover"]; // increment score for user who answered correctly
                numberOfQuestions--;
                return getRandomQuestion();
            } else {
                std::cout << "WRONG ANSWER!" << std::endl;
            }
        } else {
            std::cout << "Invalid topic or question!" << std::endl;
        }
    }
    std::string output = "Topic: " + currentTopic + "\n";
    output += "Question: " + currentQuestion + "\n";
    return output;
}




void Game::remove(){

}

void Game::readQuestions(const std::string& fileName) {
    std::ifstream file(fileName.c_str());
    if (!file) {
        std::cout << "Failed to open file: " << fileName << std::endl;
        return;
    }

    std::string line;
    std::string currentTopic;
    std::string currentQuestion;
    std::string currentAnswer;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        if (line.substr(0, 8) == "<Topic: ") {
            currentTopic = line.substr(8, line.length() - 9);
        } else if (line.substr(0, 10) == "Question: ") {
            currentQuestion = line.substr(10);
            if (currentQuestion.substr(0, 2) == "  ")
                currentQuestion = currentQuestion.substr(2);  // Remove leading whitespace
        } else if (line.substr(0, 8) == "Answer: ") {
            currentAnswer = line.substr(8);
            question[currentTopic][currentQuestion] = currentAnswer;
        }
    }
    file.close();
}




std::string Game::getRandomQuestion(){
    if (question.empty()) {
        return "No questions available.";
    }

    // Step 1: Generate a random number within the range of the map's size
    std::map<std::string, std::map<std::string, std::string> >::const_iterator topicIterator = question.begin();
    std::advance(topicIterator, std::rand() % question.size());

    // Step 2: Get the random topic
    std::string randomTopic = topicIterator->first;
    const std::map<std::string, std::string>& submap = topicIterator->second;

    if (submap.empty()) {
        return "No questions available for the selected topic.";
    }

    // Step 3: Generate a random number within the range of the submap's size
    std::map<std::string, std::string>::const_iterator questionIterator = submap.begin();
    std::advance(questionIterator, std::rand() % submap.size());

    // Step 4: Get the random question and answer
    std::string randomQuestion = questionIterator->first;
    std::string randomAnswer = questionIterator->second;

    // Step 5: Return the random question and answer
    std::string output = "Topic: " + randomTopic + "\n";
    output += "Question: " + randomQuestion + "\n";
    return output;
}

void Game::printQuestions() {
    std::map<std::string, std::map<std::string, std::string> >::iterator topicIt;
    std::map<std::string, std::string>::iterator questionIt;

    for (topicIt = question.begin(); topicIt != question.end(); ++topicIt) {
        std::cout << "Topic: <" << topicIt->first << ">" << std::endl;

        for (questionIt = topicIt->second.begin(); questionIt != topicIt->second.end(); ++questionIt) {
            std::cout << "Question: <" << questionIt->first << ">"<< std::endl;
            std::cout << "Answer: <" << questionIt->second<< ">" << std::endl;
        }

        std::cout << std::endl;
    }
}

std::string Game::caseToLower(const std::string& input) {
    std::string lowercase = input;
    for (size_t i = 0; i < lowercase.length(); ++i) {
        lowercase[i] = std::tolower(lowercase[i]);
    }
    return lowercase;
}

int Game::getNumberOfQuestions(){
    return numberOfQuestions;
}