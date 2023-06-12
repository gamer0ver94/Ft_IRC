#include "../classes/Server.hpp"

void Error(std::string str)
{
	std::cerr << str << std::endl;
	exit (0);
}

int reserved_port(unsigned int nb)
{
	unsigned int res[] = {20, 21, 22, 25, 53, 80, 443, 110, 143, 3389};
	for(int i = 0; i < 10; i++)
		if (nb == res[i] || nb == 0)
			return (0);
	return (1);
}

int main(int argc, char**argv){
	if (argc != 3){
		std::cerr << "Not enough arguments to run the program." << std::endl;
		return -1;
	}
	try{
		std::string password(argv[2]);
		unsigned int port = atoi(argv[1]);
		if (reserved_port(port))
		{
			Server *server = new Server(port, password);
			server->run();
		}
		else
			Error("Bad port input");
	}
	catch(std::exception &e){
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return 0;
}