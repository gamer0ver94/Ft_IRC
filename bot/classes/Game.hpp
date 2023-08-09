#pragma once
#include <iostream>
#include <map>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>

class Game{
    public :
        Game(std::string file, std::string channelName);
        ~Game();
        void addPlayer(std::string player);
        void remove();
        std::string run();
        std::string update(std::string message, std::string clientName);
        //getter
        int getNumberOfQuestions();
        void decrementNumberOfQuestions();
        std::string getRandomQuestion();
        //getter
		std::map<std::string, int> getPlayers();
        bool getRunning();
        std::string& getCurrentTopic();
        std::string& getCurrentQuestion();
        time_t getStart();
        std::string getAnwser();
        //setter
        void setRunning(bool state);
        void setStart(time_t time);
    private :
        std::map<std::string, int> players;
		time_t start;
        std::string currentTopic;
        std::string currentQuestion;
        bool running;
        std::string channelName;
        //A map that as key as topic and another map as value that coint question and awser;
        std::map<std::string, std::map<std::string, std::string> > question;
        int numberOfQuestions;
        void readQuestions(const std::string& fileName);
        void printQuestions();
        std::string caseToLower(const std::string& input);
		std::string extractClientName(std::string message);
		bool isAlreadyAsked(std::string question);
		std::vector<std::string> AskedQuestions;
};