#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Webserv.hpp"

class Server;

class ServerSocket
{
private:
	std::vector<struct pollfd> fds;
	
public:
	ServerSocket() : fds() {};
	~ServerSocket() {};
	ServerSocket(const ServerSocket& other) : fds(other.fds) {};
	ServerSocket& operator=(const ServerSocket& other) {
		if (this != &other) {
			fds = other.fds;
		}
		return *this;
	};

	void	serverSockSet(std::vector<struct pollfd> fds, const std::vector<Server>& ser);
	void	connectClient(std::vector<struct pollfd> fds, std::vector<Server>& ser);
};

#endif