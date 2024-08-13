#include "../inc/server.hpp"

Server::Server()
	: serverName(0), port(0), host(0), maxBodySize(1000), root(0), errorPage(0)
{}
Server::~Server(){
}

Server::Server(const Server& rhs)
	: serverName(rhs.serverName), port(rhs.port), host(rhs.host), maxBodySize(rhs.maxBodySize), root(rhs.root), errorPage(rhs.errorPage) {
}

Server&	Server::operator=(const Server& rhs) {
	if (this != &rhs) {
		serverName = rhs.serverName;
		port = rhs.port;
		host = rhs.host;
		maxBodySize = rhs.maxBodySize;
		root = rhs.root;
		errorPage = rhs.errorPage;
	}
	return *this;
}

void	Server::setName(std::string& n) {
	if (!n.empty())
		serverName = n;
}

void	Server::setPort(const std::string& p) {
	if (!p.empty())
		port = std::atoi(p.c_str());
}

void	Server::setHost(std::string& h) {
	if (!h.empty())
		host = h;
}

void	Server::setSize(const std::string& s) {
	if (!s.empty())
		maxBodySize = std::atoi(s.c_str());
}

void	Server::setRoot(std::string& r) {
	if (!r.empty())
		root = r;
}
void	Server::setError(std::string& e) {
	if (!e.empty())
		errorPage = e;
}
