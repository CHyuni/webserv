#ifndef PARSECONFIGURE_HPP
#define PARSECONFIGURE_HPP

#include "Webserv.hpp"

class ParseConfigure
{
private:
	static const std::string validKey[];
	std::vector<std::string>	_server;

public:
	ParseConfigure() : _server() {};
	~ParseConfigure() {};
	ParseConfigure(const ParseConfigure& rhs) : _server(rhs._server) {};
	ParseConfigure& operator=(const ParseConfigure& rhs) {
		if (this != &rhs)
			_server = rhs._server;
		return *this;
	};

	void				splitServer(const std::string& rhs);
	bool				checkLine(const std::string& rhs);
	std::string			lineGet(const std::string& rhs);
	std::string			getServer(const size_t& i);
	size_t				serverSize();
};


#endif

