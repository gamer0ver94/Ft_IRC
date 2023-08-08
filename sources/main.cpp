#include "../classes/Server.hpp"

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGQUIT) {
		exit(-1);
	}
	std::cout << "signal detected" << std::endl;
}

int main(int argc, char**argv){
	if (argc < 2){
		std::cerr << "Not enough arguments to run the program." << std::endl;
		return -1;
	}
	else if(argc > 3){
		std::cerr << "Too much arguments to run the program." << std::endl;
		return -1;
	}
	else if (argc == 2)
	{
		try{
			std::string password("NULL");
			std::cout << password << std::endl;
			unsigned int port = atoi(argv[1]);
			if (port <= 1023){
				std::cerr << "Port not valid, minimum 1024" << std::endl;
				return -1;
			}
			Server *server = new Server(port, password, false);
			server->run();
			delete server;
		}
		catch(std::exception &e){
		std::cerr << "Error: " << e.what() << std::endl;
		}
	}
	else
	{
		try{
			std::string password(argv[2]);
			std::cout << password << std::endl;
			unsigned int port = atoi(argv[1]);
			if (port <= 1023){
				std::cerr << "Port not valid, minimum 1024" << std::endl;
				return -1;
			}
			Server *server = new Server(port, password, true);
			server->run();
			delete server;
		}
		catch(std::exception &e){
		std::cerr << "Error: " << e.what() << std::endl;
		}
	}
	return 0;
}