#include "../inc/ServerSocket.hpp"

const std::string ServerSocket::methodList[] = {
	 "GET", "POST", "DELETE", "PUT", "PATCH", "HEAD", "OPTIONS", "TRACE", "CONNECT", ""
};

void	ServerSocket::serverSockSet(const std::vector<Server>& ser) {
	std::map<u_int16_t, int> portCheck;
	size_t size = ser.size();
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < ser[i].getPortSize(); ++j) {
			if (!portCheck.empty()) {
				if (portCheck.find(ser[i].getPort(j)) != portCheck.end())
					continue ;
			}
			portCheck[ser[i].getPort(j)] = 1;
			int listenSock = socket(AF_INET, SOCK_STREAM, 0);
			if (listenSock == 0)
				throw std::runtime_error("Socket creation failed");
			
			int flags = fcntl(listenSock, F_GETFL, 0);
			fcntl(listenSock, F_SETFL, flags | O_NONBLOCK);

			int opt = 1;
			if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
				throw std::runtime_error("setsockopt failed");
			
			struct sockaddr_in address;
			std::string ip = ser[i].getHost();
			in_addr_t ipAddr = inet_addr(ip.c_str());
			if (ipAddr == INADDR_NONE)
				throw std::invalid_argument("Address Invalid");
			memset(&address, 0, sizeof(address));
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = ipAddr;
			address.sin_port = htons(ser[i].getPort(j));

			if (bind(listenSock, (struct sockaddr *)&address, sizeof(address)) < 0)
				throw std::runtime_error("Bind Failed");
			
			if (listen(listenSock, SOMAXCONN) < 0)
				throw std::runtime_error("Listen Failed");
			
			struct pollfd listen_pollfd;
			listen_pollfd.fd = listenSock;
			listen_pollfd.events = POLLIN;
			_fds.push_back(listen_pollfd);
		}
	}
}

void	ServerSocket::connectClient(std::vector<Server>& ser) {
	std::vector<bool> response_needed(_fds.size(), false);
	size_t size = _fds.size();
	while (true) {
		int ret = poll(&_fds[0], _fds.size(), 0);
		if (ret < 0)
			throw std::runtime_error("poll Failed");
		for (size_t i = 0; i < size; ++i) {
			if (_fds[i].revents & POLLIN) {
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_sock = accept(_fds[i].fd, (struct sockaddr*)&client_addr, &client_len);
				if (client_sock < 0) {
					std::cerr << "Aceept failed" << std::endl;
					continue ;
				}
				int client_flags = fcntl(client_sock, F_GETFL, 0);
				fcntl(client_sock, F_SETFL, client_flags | O_NONBLOCK);

				struct pollfd client_pollfd;
				client_pollfd.fd = client_sock;
				client_pollfd.events = POLLIN;
				_fds.push_back(client_pollfd);
				response_needed.push_back(false);
			}
		}
		clientRequest(response_needed, ser, size);
	}
}

void	ServerSocket::clientRequest(std::vector<bool>& response_needed, std::vector<Server>& ser, const size_t& size) {
	for (size_t i = size; i < _fds.size();) {
		Client cli;
		int fd = _fds[i].fd;
        if (_fds[i].revents & POLLIN) {
			std::vector<char> totalData = readClientData(_fds[i].fd, ser, cli);
			if (totalData.empty()) {
				close(_fds[i].fd);
                _fds.erase(_fds.begin() + i);
                response_needed.erase(response_needed.begin() + i);
				_cliMap.erase(fd);
				continue;
			}
			response_needed[i] = true;
            _fds[i].events |= POLLOUT;
			cli.setMethod(determineHttpMethod(totalData));
			cli.setData(totalData);
			_cliMap[fd] = cli;
		}
        if (_fds[i].revents & POLLOUT && response_needed[i]) {
            handleHttpMethod(fd, ser);
			response_needed[i] = false;
			_fds[i].events &= ~POLLOUT;
        }
		++i;
    }
}

std::vector<char> ServerSocket::readClientData(int fd, std::vector<Server>& ser, Client& cli) {
    std::vector<char> buffer(8192);
    std::vector<char> totalData;
    size_t totalRead = 0;

    while (true) {
        int valread = recv(fd, &buffer[0], buffer.size(), 0);
        if (valread > 0) {
            totalData.insert(totalData.end(), buffer.begin(), buffer.begin() + valread);
            totalRead += valread;
        } else {
            if (valread < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
				return std::vector<char>();
			}
            break;
        }
        if (cli.getSize() == __SIZE_MAX__) {
            configureCheck(totalData, ser, cli);
        } else {
            if (totalRead > cli.getSize())
                throw std::runtime_error("maxBodySize exceeded. This exception will trigger an error page.");
        }
    }
    return totalData;
}

bool	ServerSocket::configureCheck(std::vector<char> data, std::vector<Server>& ser, Client& cli) {
	std::string dataStr(data.begin(), data.end());
	std::istringstream iss(dataStr);
	std::string line;

	if (!std::getline(iss, line) || !checkRequestLine(line, cli))
		throw std::runtime_error("400 or getline Error");
	bool hostFound = false;
	while (std::getline(iss, line)) {
		if (line.substr(0, 6) == "Host: ") {
			hostFound = true;
			if (!checkHostHeader(line.substr(6), ser, cli)) {
				throw std::runtime_error("host 400 Error exception");
			}
			break ;
		}
	}
	if (!hostFound)
		throw std::runtime_error("host missing");
	return true;
}

bool	ServerSocket::checkRequestLine(const std::string& line, Client& cli) {
	std::istringstream iss(line);
	std::string method, uri, httpVersion;
	iss >> method >> uri >> httpVersion;
	bool methodCheck = false;

	for (size_t i = 0; i < 9; ++i) {
		if (method == methodList[i]) {
			methodCheck = true;
			break ;
		}
	}
	if (!methodCheck)
		return false;
	if (uri.empty() || uri[0] != '/')
		return false;
	if (httpVersion != "HTTP/1.0" && httpVersion != "HTTP/1.1")
		return false;
	cli.setVersion(httpVersion);
	cli.setOriginUri(uri);
	return true;
}

bool	ServerSocket::checkHostHeader(const std::string& hostLine, const std::vector<Server>& ser, Client& cli) {
	std::string::size_type pos = hostLine.find(':');
	std::string serverName = (pos != std::string::npos) ? hostLine.substr(0, pos) : hostLine;
	long portNum = 0;

	if (pos != std::string::npos) {
		std::string portStr = hostLine.substr(pos + 1);
		char* endptr;
		portNum = std::strtol(portStr.c_str(), &endptr, 10);
		if ((*endptr != '\r' && *endptr != '\n' && *endptr != ';' && *endptr != '\0') || portNum <= 0 || portNum > 65535) {
			return false;
		}
	} else return false;

	for (size_t i = 0; i < ser.size(); ++i) {
		for (size_t j = 0; j < ser[i].getNameSize(); ++j) {
			if (serverName == ser[i].getName(j)) {
				for (size_t k = 0; k < ser[i].getPortSize(); ++k) {
					if (portNum == ser[i].getPort(k)) {
						cli.setSize(ser[i].getSize());
						cli.setIndex(i);
						cli.setPort(portNum);
						return true;
					}
				}
			}
		}
	}
	return false;
}

std::string ServerSocket::determineHttpMethod(const std::vector<char>& totalData) {
    std::string dataStr(totalData.begin(), totalData.end());
    for (size_t i = 0; i < 9; ++i) {
        size_t pos = dataStr.find(methodList[i]);
        if (pos != std::string::npos) {
            if (methodList[i] != "GET" && methodList[i] != "POST" && methodList[i] != "DELETE")
                throw std::runtime_error("404 error. This exception will trigger an error page");
            return methodList[i];
        }
    }
    throw std::runtime_error("Unknown HTTP method");
}

void ServerSocket::handleHttpMethod(int fd, std::vector<Server>& ser) {
	uriMapping(fd, ser);
    if (_cliMap[fd].getMethod() == "GET") {
        handleGetRequest(fd, ser);
    } else if (_cliMap[fd].getMethod() == "POST") {
        // handlePostRequest(cli, data, ser);
    } else if (_cliMap[fd].getMethod() == "DELETE") {
        // handleDeleteRequest(cli, data, ser);
    }
}

void ServerSocket::handleGetRequest(int fd, std::vector<Server>& ser) {
	std::string response;
	int			result = isDirectory(_cliMap[fd].getMappingUri());
	if (result == 1) {
		response = directoryHandler(ser, fd);
	} else if (result == -1) {
		response = errorResponse("404", ser, fd);
	}
	// else
	// 	fileHandler(response);
	int	valsend = send(fd, response.c_str(), response.length(), 0);
	if (valsend > 0) {
		std::cout << "Response sent" << std::endl;
	};
}

std::string ServerSocket::getGMTDate() {
	time_t now = time(0);
	struct tm *gmtm = gmtime(&now);
	char date_str[100];
	strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", gmtm);
	return std::string(date_str);
}

int	ServerSocket::isDirectory(const std::string& uri) {
	size_t dotPos = uri.find_last_of('.');
	size_t slashPos = uri.find_last_of('/');
	if (dotPos != std::string::npos && dotPos > slashPos) {
		return 0;
	}
	struct stat info;
	if (stat(uri.c_str(), &info) != 0) {
		return -1;
	} else if (info.st_mode & S_IFDIR) {
		return 1;
	} else {
		return 0;
	}
	return 0;
}

std::string	ServerSocket::directoryHandler(std::vector<Server>& ser, int fd) {
	struct stat			info;
	size_t				index = _cliMap[fd].getIndex();
	Location			loca = ser[index].getLocation(_cliMap[fd].getOriginUri());
	std::string			path = loca.getRoot() + "/";
	std::ostringstream	oss;
	std::string			tmp = _cliMap[fd].getData();
	std::string			response;

	for (size_t i = 0; i < loca.getIndexSize(); ++i) {
		std::string temp = path + loca.getIndex(i);
		if (stat(temp.c_str(), &info) == 0) {
			path = temp;
			long size = info.st_size;
			oss << size;
		}
	}
	if (path == loca.getRoot() + "/") {
		return errorResponse("404", ser, fd);
	}
	response = "HTTP/1.1 200 OK\r\nDate: ";
	response += getGMTDate() + "\r\n";

	if (path.find("htm") != std::string::npos) {
		response += "Content-Type: text/html\r\nContent-Length: ";
		response += oss.str() + "\r\nConnection: ";
	}
	response += connectionSelect(fd);
	response += getContent(path);
	return response;
}

// void	ServerSocket::fileHandler(std::string& response) {

// }

std::string	ServerSocket::errorResponse(const std::string& number, std::vector<Server>& ser, int fd) {
	struct stat info;
	std::string response;
	std::ostringstream oss;
	std::string tmp = _cliMap[fd].getData();
	
	if (number == "404") {
		std::string path = ser[_cliMap[fd].getIndex()].getError("404", 0);
		response = "HTTP/1.1 404 Not Found\r\nDate: ";
		response += getGMTDate() + "\r\nContent-Type: text/html\r\nContent-Length: ";
		if (stat(path.c_str(), &info) == 0) {
			long size = info.st_size;
			oss << size;
		}
		response += oss.str() + "\r\nConnection: ";
		response += connectionSelect(fd);
		response += getContent(path);
	}
	return response;
}

std::string ServerSocket::connectionSelect(int fd) {
	std::string tmp = _cliMap[fd].getData();
	std::string response;
	size_t pos = tmp.find("Connection: ");
	if (pos != std::string::npos) {
		size_t start = pos + 12;
		size_t end = tmp.find("\r\n", start);
		response = tmp.substr(start, end - start) + "\r\n\r\n";
	} else {
		if (_cliMap[fd].getVersion() == "HTTP/1.1")
			response += "keep-alive\r\n";
		else if (_cliMap[fd].getVersion() == "HTTP/1.0")
			response += "close\r\n\r\n";
	}
	return response;
}

std::string ServerSocket::getContent(const std::string& path) {
	std::ifstream	file(path.c_str());
	std::string		line;
	std::string		result;
	while (getline(file, line)) {
		result += line + "\n";
	}
	file.close();
	return result;
}

void	ServerSocket::uriMapping(int fd, std::vector<Server>& ser) {
	std::string	uri = _cliMap[fd].getOriginUri();
	std::string	mapUri;
	int			size = std::count(uri.begin(), uri.end(), '/');
	int			index = _cliMap[fd].getIndex();

	for (int i = 0; i < size; ++i) {
		std::string	temp;
		size_t		posTemp;
		size_t		pos;
		if (i == 0)
			temp = uri;
		else {
			posTemp = uri.find_last_of('/');
			pos = uri.find_last_of('/', posTemp - i + 1);
			temp = uri.substr(0, pos);
		}
		Location loca = ser[index].getLocation(temp);
		if (!loca.getPath().empty()) {
			if (i == 0) {
				mapUri = loca.getRoot();
			}
			else {
				mapUri = loca.getRoot();
				mapUri += uri.substr(pos);
			}
			_cliMap[fd].setMappingUri(mapUri);
			return ;
		}
	}
	mapUri = ser[index].getLocation("/").getRoot() + uri;
	_cliMap[fd].setMappingUri(mapUri);
}