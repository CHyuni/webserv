#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Server.hpp"

class Location : public Server
{
private:
	std::string _path;

public:
	Location() : Server(), _path("") {};
	Location(const Server& other) : Server(other), _path("") {};
	Location(const Location& other) : Server(other), _path(other._path) {};
	Location& operator=(const Location& other) {
		if (this != &other) {
			Server::operator=(other);
			_path = other._path;
		}
		return *this;
	}
	virtual ~Location() {};

	std::string	getPath() const;
	void		setPath(const std::string& path);
};

#endif