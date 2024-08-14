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

void	Server::setName(const std::string& n) {
	if (!n.empty())
		_serverName.push_back(n);
}

void	Server::setPort(const std::string& p) {
	if (!p.empty())
		_port = std::atoi(p.c_str());
}

void	Server::setHost(const std::string& h) {
	if (!h.empty())
		_host = h;
}

void	Server::setSize(const std::string& s) {
	if (!s.empty()) {
		char*	endptr;
		long	temp = strtol(s.c_str(), &endptr, 10);

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

void	Server::setRoot(const std::string& r) {
	if (!r.empty())
		_root = r;
}

void	Server::setError(const std::string& e) {
	if (!e.empty()) {
		std::string	key;
		std::string	value;
		size_t		i = 0;

		while (i < e.length() && !isspace(e[i]) && e[i] != ';')
			key += e[i++];
		if (key.empty() || e[i] == ';')
			return ; // < throw expected
		while (i < e.length() && isspace(e[i]))
			i++;
		while (i < e.length() && e[i] != ';')
			value += e[i++];
		if (value.empty())
			return ; // < throw expected
		_errorPage[key] = value;
	}
}

void	Server::setIndex(const std::string& i) {
	if (!i.empty()) {
		std::string	temp;
		size_t		idx = 0;

		while (idx < i.length()) {
			if (isspace(i[idx]) || i[idx] == ';') {
				if (!temp.empty()) {
					_index.push_back(temp);
					temp.clear();
				}
				if (i[idx] == ';') break;
			} else {
				temp += i[idx];
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