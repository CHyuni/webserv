#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Webserv.hpp"

class Server;

class ServerSocket
{
private:
	std::vector<struct pollfd> _fds;
	std::string methodList[9] = {"GET", "POST", "DELETE", "PUT", "PATCH", "HEAD", "OPTIONS", "TRACE", "CONNECT"};

	bool						checkRequestLine(const std::string& line);
	bool						checkHostHeader(const std::string& hostline, const std::vector<Server>& ser, size_t& limit);
	
public:
	ServerSocket() : _fds() {};
	~ServerSocket() {};
	ServerSocket(const ServerSocket& other) : _fds(other._fds) {};
	ServerSocket& operator=(const ServerSocket& other) {
		if (this != &other) {
			_fds = other._fds;
		}
		return *this;
	};

	void	serverSockSet(const std::vector<Server>& ser);
	void	connectClient(std::vector<Server>& ser);
	bool	configureCheck(const std::vector<char> data, std::vector<Server>& ser, size_t& limit);
};

#endif