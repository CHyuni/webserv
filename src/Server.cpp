#include "../inc/Server.hpp"
#include "../inc/Location.hpp"

Server::Server()
	: _serverName(), _port(), _host(""), _maxBodySize(1024 * 1024), _root(""), 
	_methods(), _autoidx(false), _index(), _errorPage(), _locationMap() {}

Server::~Server(){}

Server::Server(const Server& rhs)
	: _serverName(rhs._serverName), _port(rhs._port), _host(rhs._host), _maxBodySize(rhs._maxBodySize), _root(rhs._root), 
	_methods(rhs._methods), _autoidx(rhs._autoidx), _index(rhs._index), _errorPage(rhs._errorPage) {
		std::map<std::string, Location>::const_iterator it;
		for (it = rhs._locationMap.begin(); it != rhs._locationMap.end(); ++it)
			_locationMap[it->first] = it->second;
	}

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
        _locationMap.clear();
		std::map<std::string, Location>::const_iterator rhs_it;
        for (rhs_it = rhs._locationMap.begin(); rhs_it != rhs._locationMap.end(); ++rhs_it) {
            _locationMap[rhs_it->first] = rhs_it->second;
        }
	}
	return *this;
}

bool isValidOctet(const char* str, char** endptr) {
	errno = 0;
	long value = std::strtol(str, endptr, 10);
	return errno != ERANGE && value >= 0 && value <= 255;
}

bool isValidStrMini(const std::string& str) {
	size_t	pos = str.find(";");
	if (pos != std::string::npos && str[pos + 1] != '\0') {
		while (str[++pos]) {
			if (!std::isspace(str[pos]))
				return false;
		}
	}
	return true;
}

bool isValidStr(const std::string& str) {
	bool	flags = false;
	int		cnt = 0;
	if (!isValidStrMini(str))
		return false;
	size_t	pos = str.find(";");
	if (pos == std::string::npos) {
		pos = str.length();
	}
	for (std::string::const_iterator it = str.begin(); it != str.begin() + pos; ++it) {
		if (flags == false) {
			if (!std::isspace(*it)) {
				flags = true;
				cnt++;
			}
			continue ;
		}
		if (flags && std::isspace(*it))
			flags = false;
	}
	if (cnt != 1)
		return false;
	return true;
}


void	Server::setName(const std::string& str) {
	std::string	temp;
	size_t		i = 0;
	if (!isValidStrMini(str))
		return ; // << throw expected
	while (i < str.length()) {
		while (i < str.length() && std::isspace(str[i]))
			i++;
		while (i < str.length() && !std::isspace(str[i]) && str[i] != ';')
			temp += str[i++];
		if (!temp.empty()) {
			_serverName.push_back(temp);
			temp.clear();
		}
		if (str[i++] == ';')
			break ;
	}
	if (_serverName.empty())
		return ;// < throw expected
}

void	Server::setPort(const std::string& str) {
	std::string temp;
	size_t		i = 0;
	long		check;

	while (i < str.length() && std::isspace(str[i]))
		i++;
	while (i < str.length() && !std::isspace(str[i]) && str[i] != ';')
		temp += str[i++];
	if (!temp.empty()) {
		char* endptr;
		errno = 0;
		check = std::strtol(temp.c_str(), &endptr, 10);
		if (endptr == temp.c_str() || errno == ERANGE || (check < 0 || check > 65535))
			return ; // throw expected
	}
	while (i < str.length() && std::isspace(str[i]))
		i++;
	if (i < str.length()) {
		if (str[i] == ';') {
			i++;
			while (i < str.length() && std::isspace(str[i]))
				i++;
			if (i < str.length())
				return ; // throw expected
		} else {
			return ; // throw expected
		}
	}
	_port.push_back(static_cast<u_int16_t>(check));
}

void	Server::setHost(const std::string& str) {
	std::string temp;
	size_t		i = 0;
	while (i < str.length() && std::isspace(str[i]))
		i++;
	while (i < str.length() && !std::isspace(str[i]) && str[i] != ';')
		temp += str[i++];
	while (i < str.length() && std::isspace(str[i]))
		i++;
	if (i < str.length()) {
		if (str[i] == ';') {
			i++;
			while (i < str.length() && std::isspace(str[i]))
				i++;
			if (i < str.length()) 
				return ; // throw expected
		} else
			return ; // throw expected
	}
	if (!temp.empty()) {
		char* endptr;
		const char* current = temp.c_str();
		for (int j = 0; j < 4; ++j) {
			if (!isValidOctet(current, &endptr))
				return ; // throw expected
			if (j < 3) {
				if (*endptr != '.')
					return ; // throw expected
				current = endptr + 1;
			} else {
				while (*endptr != '\0') {
					if (!std::isspace(*endptr))
						return ;// throw expected
					endptr++;
				}
			}
		}
		_host = temp;
	} else
		return ; // throw expected
}

void	Server::setSize(const std::string& str) {
	char*	endptr;
	long	temp = std::strtol(str.c_str(), &endptr, 10);

	if (temp > 0 && *endptr != '\0') {
		std::string unit;
		unit.assign(endptr);
		for (std::string::iterator it = unit.begin(); it != unit.end();) {
			if (std::isspace(*it) || *it == ';')
				it = unit.erase(it);
			else
				it++;
		}
		if (unit == "M" || unit == "MB") {
		temp *= 1024 * 1024;
		}else if (unit == "K" || unit == "KB") {
			temp *= 1024;
		}else if (unit == "G" || unit == "GB") {
			temp *= 1024 * 1024 * 1024;
		} else
			return ; // throw expected
		_maxBodySize = temp;
	}
}

void	Server::setRoot(const std::string& str) {
	if (!isValidStr(str))
		return ; // throw expected
	std::string temp;
	size_t		i = 0;
	while (i < str.length() && std::isspace(str[i]))
		i++;
	while (i < str.length() && !std::isspace(str[i]) && str[i] != ';')
		temp += str[i++];
	_root = temp;
}

void	Server::setError(const std::string& str) {
	std::string	key;
	std::string	value;
	size_t		i = 0;
	if (!isValidStrMini(str))
		return ; // << throw expected
	while (i < str.length() && std::isspace(str[i]))
		i++;
	while (i < str.length() && !std::isspace(str[i]) && str[i] != ';')
		key += str[i++];
	if (key.empty() || str[i] == ';')
		return ; // < throw expected
	while (i < str.length()) {
		while (i < str.length() && std::isspace(str[i]))
			i++;
		while (i < str.length() && !std::isspace(str[i]) && str[i] != ';')
			value += str[i++];
		if (!value.empty()) {
			_errorPage[key].push_back(value);
			value.clear();
		}
		if (str[i++] == ';')
			break ;
	}
}

void	Server::setIndex(const std::string& str) {
	std::string	temp;
	size_t		i = 0;
	if (!isValidStrMini(str))
		return ; // << throw expected
	while (i < str.length()) {
		while (i < str.length() && std::isspace(str[i]))
			i++;
		while (i < str.length() && !std::isspace(str[i]) && str[i] != ';')
			temp += str[i++];
		if (!temp.empty()) {
			_index.push_back(temp);
			temp.clear();
		}
		if (str[i++] == ';')
			break ;
	}
	if (_index.empty())
		return ;// < throw expected
}

void	Server::setAutoidx(const std::string& str) {
	std::string temp;
	size_t		i = 0;
	if (!isValidStr(str))
		return ; // throw expected
	while (i < str.length() && std::isspace(str[i]))
			i++;
	while (i < str.length() && !std::isspace(str[i]) && str[i] != ';')
			temp += str[i++];
	if (temp == "on")
		_autoidx = true;
	else
		_autoidx = false;
}

void	Server::setMethods(const std::string& str) {
	std::string	temp;
	size_t		i = 0;
	if (!isValidStrMini(str))
		return ; // << throw expected
	while (i < str.length()) {
		while (i < str.length() && std::isspace(str[i]))
			i++;
		while (i < str.length() && !std::isspace(str[i]) && str[i] != ';')
			temp += str[i++];
		if (!temp.empty()) {
			_methods.push_back(temp);
			temp.clear();
		}
		if (str[i++] == ';')
			break ;
	}
	if (_methods.empty())
		return ;// < throw expected
}

void	Server::setLocation(const std::string& str, const Location& loca) {
	_locationMap[str] = loca;
};

std::string	Server::getName(size_t i) const {
	if (_serverName.empty())
		return "server_name empty.";
	return _serverName[i];
}

u_int16_t	Server::getPort(size_t i) const {
	if (_port.empty())
		return 0;
	return _port[i];
}

std::string	Server::getHost() const {
	if (_host.empty())
		return "host empty.";
	return _host;
};

long		Server::getSize() const {
	return _maxBodySize;
};

std::string	Server::getRoot() const {
	if (_root.empty())
		return "root empty.";
	return _root;
};

std::string	Server::getMethods(size_t i) const {
	if (_methods.empty())
		return "methods empty.";
	return _methods[i];
};

bool		Server::getAutoidx() const {
	return _autoidx;
};

std::string	Server::getIndex(size_t i) const {
	if (_index.empty())
		return "Error: index empty.";
	return _index[i];
};

std::string	Server::getError(const std::string& rhs, size_t i) {
	if (_errorPage.empty())
		return "ErrorPage empty.";
	if (_errorPage.find(rhs) == _errorPage.end())
		return "Not found key.";
	std::vector<std::string> temp = _errorPage[rhs];
	return	temp[i];
};

Location Server::getLocation(const std::string& rhs) {
	if (_locationMap.empty())
		throw std::invalid_argument("LocationMap empty.");
	if (_locationMap.find(rhs) == _locationMap.end())
		throw std::invalid_argument("Key invalid.");
	Location temp = _locationMap[rhs];
	return temp;
}

size_t	Server::getPortSize() const {
	return _port.size();
}

size_t Server::getNameSize() const {
	return _serverName.size();
}