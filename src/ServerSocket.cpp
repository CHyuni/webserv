#include "../inc/ServerSocket.hpp"
#include "../inc/Server.hpp"

void	ServerSocket::serverSockSet(const std::vector<Server>& ser) {
	std::map<u_int16_t, int> portCheck;
	size_t size = ser.size();
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < ser[i].getPortSize(); ++j) {
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

		for (size_t i = size; i < _fds.size(); ++i) {
			if (_fds[i].revents & POLLIN) {
				std::vector<char> buffer(8192);
				std::vector<char> totalData;
				size_t totalRead = 0;
				size_t limitRead = 0;
				bool check = false;

				while (true) {
					int valread = recv(_fds[i].fd, &buffer[0], buffer.size(), 0);
					if (valread > 0) {
						totalData.insert(totalData.end(), buffer.begin(), buffer.begin() + valread);
						totalRead += valread;
					} else {
						if (valread < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
							throw std::runtime_error("Error reading from socket");
						break ;
					}
					if (!check) {
						ServerSocket::configureCheck(totalData, ser, limitRead);
						check = true;
					} else {
						if (totalRead > limitRead)
							throw std::runtime_error("maxBodySize exceeded. This exception will trigger an error page.");
					}
				}
				std::string dataStr(totalData.begin(), totalData.end());
				std::string methodStr;
				for (size_t i = 0; i < 9; ++i) {
					size_t pos = dataStr.find(methodList[i]);
					if (pos != std::string::npos) {
						if (methodList[i] != "GET" && methodList[i] != "POST" && methodList[i] != "DELETE")
							throw std::runtime_error("404 error. This exception will trigger an error page");
						methodStr = methodList[i];
						break ;
					}
				}
				if (methodStr == "GET") {
					
				} else if (methodStr == "POST") {

				} else if (methodStr == "DELETE") {

				}
			}
		}
	}
}

bool	ServerSocket::configureCheck(std::vector<char> data, std::vector<Server>& ser, size_t& limit) {
	std::string dataStr(data.begin(), data.end());
	std::istringstream iss(dataStr);
	std::string line;

	if (!std::getline(iss, line) || !checkRequestLine(line))
		throw std::runtime_error("400 or getline Error");
	bool hostFound = false;
	while (std::getline(iss, line)) {
		if (line.substr(0, 6) == "Host: ") {
			hostFound = true;
			if (!checkHostHeader(line.substr(6), ser, limit)) {
				throw std::runtime_error("host 400 Error exception");
			}
			break ;
		}
	}
	if (!hostFound)
		throw std::runtime_error("host missing");
	return true;
}

bool	ServerSocket::checkRequestLine(const std::string& line) {
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
	return true;
}

bool	ServerSocket::checkHostHeader(const std::string& hostLine, const std::vector<Server>& ser, size_t& limit) {
	std::string::size_type pos = hostLine.find(':');
	std::string serverName = (pos != std::string::npos) ? hostLine.substr(0, pos) : hostLine;
	long portNum = 0;

	if (pos != std::string::npos) {
		std::string portStr = hostLine.substr(pos + 1);
		char* endptr;
		portNum = std::strtol(portStr.c_str(), &endptr, 10);
		if ((*endptr != ';' && *endptr != '\0') || portNum <= 0 || portNum > 65535) {
			return false;
		}
	} else return false;

	for (size_t i = 0; i < ser.size(); ++i) {
		for (size_t j = 0; j < ser[i].getNameSize(); ++j) {
			if (serverName == ser[i].getName(j)) {
				for (size_t k = 0; k < ser[i].getPortSize(); ++k) {
					if (portNum == ser[i].getPort(k)) {
						limit = ser[i].getSize();
						return true;
					}
				}
			}
		}
	}
	return false;
}