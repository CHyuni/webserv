#include "../inc/Server.hpp"

Server::Server()
	: _serverName(), _port(0), _host(0), _maxBodySize(1024 * 1024), _root(0), 
	_methods(), _autoidx(false), _index(), _errorPage() {}
Server::~Server(){
}

Server::Server(const Server& rhs)
	: _serverName(rhs._serverName), _port(rhs._port), _host(rhs._host), _maxBodySize(rhs._maxBodySize), _root(rhs._root), 
	_methods(rhs._methods), _autoidx(rhs._autoidx), _index(rhs._index), _errorPage(rhs._errorPage) {}

Server&	Server::operator=(const Server& rhs) {
	if (this != &rhs) {
		_serverName = rhs._serverName;
		_port = rhs._port;
		_host = rhs._host;
		_maxBodySize = rhs._maxBodySize;
		_root = rhs._root;
		_methods = rhs._methods;
		_autoidx = rhs._autoidx;
		_index = rhs._index;
		_errorPage = rhs._errorPage;
	}
	return *this;
}

void	Server::setName(const std::string& str) {
	if (!str.empty())
		_serverName.push_back(str);
}

void	Server::setPort(const std::string& str) {
	if (!str.empty())
		_port = std::atoi(str.c_str());
}

void	Server::setHost(const std::string& str) {
	if (!str.empty())
		_host = str;
}

void	Server::setSize(const std::string& str) {
	if (!str.empty()) {
		char*	endptr;
		long	temp = strtol(str.c_str(), &endptr, 10);

		if (temp > 0 && *endptr != '\0') {
			std::string unit;
			unit.assign(endptr);

			if (unit == "M" || unit == "MB") {
			temp *= 1024 * 1024;
			}else if (unit == "K" || unit == "KB") {
				temp *= 1024;
			}else if (unit == "G" || unit == "GB") {
				temp *= 1024 * 1024 * 1024;
			}
			_maxBodySize = temp;
		}
	}
}

void	Server::setRoot(const std::string& str) {
	if (!str.empty())
		_root = str;
}

void	Server::setError(const std::string& str) {
	if (!str.empty()) {
		std::string	key;
		std::string	value;
		size_t		i = 0;

		while (i < str.length() && !isspace(str[i]) && str[i] != ';')
			key += str[i++];
		if (key.empty() || str[i] == ';')
			return ; // < throw expected
		while (i < str.length() && isspace(str[i]))
			i++;
		while (i < str.length() && str[i] != ';')
			value += str[i++];
		if (value.empty())
			return ; // < throw expected
		_errorPage[key] = value;
	}
}

void	Server::setIndex(const std::string& str) {
	if (!str.empty()) {
		std::string	temp;
		size_t		idx = 0;

		while (idx < str.length()) {
			if (isspace(str[idx]) || str[idx] == ';') {
				if (!temp.empty()) {
					_index.push_back(temp);
					temp.clear();
				}
				if (str[idx] == ';') break;
			} else {
				temp += str[idx];
			}
			idx++;
		}
		if (!temp.empty()) {
			_index.push_back(temp);
		}
		if (_index.empty())		
			return ;// < throw expected
	}
}

void	Server::setAutoidx(const std::string& str) {
	if (str == "on")
		_autoidx = true;
	else
		_autoidx = false;
}

void	Server::setMethods(const std::string& str) {
	std::string	temp;
	size_t		i = 0;

	while (i < str.length() && isspace(str[i]) && str[i] != ';')
		i++;
	if (str[i] == '\0' || str[i] == ';')
		return ; // < throw expected
	while (i < str.length() && str[i] != ';') {
		while (i < str.length() && isspace(str[i]))
			i++;
		while (i < str.length() && str[i] != ';' && !isspace(str[i]) ) {
			temp += str[i++];
		}
		if (temp.empty())
			return ; // < throw expected
		_methods.push_back(temp);
		temp.clear();
	}
	if (str[i] == ';' && str[i + 1] != '\0')
		return ; // < throw expected
}