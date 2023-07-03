#pragma once
#include <iostream>
#include <map>
#include <fstream>
#include <cstdlib>

class Game{
    public :
        Game(std::string file, std::string channelName);
        ~Game();
        void addPlayer(std::string player);
        void remove();
        std::string run();
        std::string update(std::string message, std::string clientName);
        bool running;
        std::string currentTopic;
        std::string currentQuestion;
        //getter
        int getNumberOfQuestions();
		std::map<std::string, int> getPlayers();
    private :
        std::string channelName;
        std::map<std::string, int> players;
        //A map that as key as topic and another map as value that coint question and awser;
        std::map<std::string, std::map<std::string, std::string> > question;
        int numberOfQuestions;
        void readQuestions(const std::string& fileName);
        std::string getRandomQuestion();
        void printQuestions();
        std::string caseToLower(const std::string& input);
		std::string extractClientName(std::string message);
};