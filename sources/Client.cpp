#include "../classes/Client.hpp"

Client::Client(int socket, std::string nickName, std::string userName, std::string hostName, std::string serverHostName, std::string realName)
	: socketFd(socket), nickName(nickName), userName(userName), hostName(hostName), serverHostName(serverHostName), realName(realName), op(false)
{
	std::cout << "constructor" << op << std::endl;
}
Client::~Client()
{
	// close(socketFd);
}

int Client::getSocketFd()
{
	return (this->socketFd);
}

std::string Client::getNickName()
{
	return (this->nickName);
}

std::string Client::getUserName()
{
	return (this->userName);
}

std::string Client::getHostName()
{
	return (this->hostName);
}

std::string Client::getServerHostName()
{
	return (this->serverHostName);
}

std::string Client::getRealName()
{
	return (this->realName);
}

std::string Client::getUserHost()
{
	return (this->userHost);
}

std::string Client::getLocalHost()
{
	return (this->localHost);
}

bool Client::getOperator()
{
	return (this->op);
}

void Client::setNickname(std::string newNickName)
{
	this->nickName = newNickName;
	return ;
}

void Client::setSocketFd(int socket)
{
	this->socketFd = socket;
	return ;
}

void Client::setOperator(bool status)
{
	this->op = status;
	return ;
}
