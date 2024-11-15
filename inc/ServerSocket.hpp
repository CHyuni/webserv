#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Webserv.hpp"
#include "Client.hpp"
#include "Location.hpp"


class ServerSocket
{
private:
	std::vector<struct pollfd> _fds;
	std::map<int, Client> _cliMap;
	static const std::string methodList[];

	bool				checkRequestLine(const std::string& line, Client& cli);
	bool				checkHostHeader(const std::string& hostline, const std::vector<Server>& ser, Client& cli);
	void				clientRequest(std::vector<bool>& response_needed, std::vector<Server>& ser, const size_t& size);
	std::vector<char>	readClientData(int fd, std::vector<Server>& ser, Client& cli);
	std::string			determineHttpMethod(const std::vector<char>& totalData);
	void				handleHttpMethod(int fd, std::vector<Server>& ser);
	void				handleGetRequest(int fd, std::vector<Server>& ser);
	void				handlePostRequest(Client& cli, const std::string& data, std::vector<Server>& ser);
	void				handleDeleteRequest(Client& cli, const std::string& data, std::vector<Server>& ser);
	int					isDirectory(const std::string& uri);
	std::string			directoryHandler(std::vector<Server>& ser, int fd);
	void				fileHandler(std::string& response);
	std::string			connectionSelect(int fd);
	std::string			getContent(const std::string& path);
	void				uriMapping(int fd, std::vector<Server>& ser);
	void				closeClient(size_t index, int fd, std::vector<bool>& response_needed);
	
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

	void			serverSockSet(const std::vector<Server>& ser);
	void			connectClient(std::vector<Server>& ser);
	bool			configureCheck(const std::vector<char> data, std::vector<Server>& ser, Client& cli);
	std::string		errorResponse(const std::string& number, std::vector<Server>& ser, int fd);
	std::string		getGMTDate();
};

#endif