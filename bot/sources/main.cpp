#include "../classes/Bot.hpp"

int main(int argc, char**argv){
	if (argc < 3){
		std::cerr << "Not enough arguments to run the program." << std::endl;
		return -1;
	}
	try{
        std::string botName(argv[1]);
        std::string serverIp = argv[2];
		int port = atoi(argv[3]);
		std::string password;
		if (argv[4]){
			password = argv[4];
		}
		else{
		}
		Bot *bot = new Bot(botName, serverIp, port, password);
		bot->run();
		delete bot;
	}
	catch(std::exception &e){
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return 0;
}