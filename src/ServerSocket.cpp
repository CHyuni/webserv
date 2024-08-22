#include "../inc/ServerSocket.hpp"
#include "../inc/Server.hpp"

void	ServerSocket::serverSockSet(std::vector<struct pollfd> fds, const std::vector<Server>& ser) {
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
			fds.push_back(listen_pollfd);
		}
	}
}

void	ServerSocket::connectClient(std::vector<struct pollfd> fds, std::vector<Server>& ser) {
	std::vector<bool> response_needed(fds.size(), false);
	size_t size = fds.size();
	while (true) {
		int ret = poll(&fds[0], fds.size(), -1);
		if (ret < 0)
			throw std::runtime_error("poll Failed");
		for (size_t i = 0; i < size; ++i) {
			if (fds[i].revents & POLLIN) {
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_sock = accept(fds[i].fd, (struct sockaddr*)&client_addr, &client_len);
				if (client_sock < 0) {
					std::cerr << "Aceept failed" << std::endl;
					continue ;
				}
				int client_flags = fcntl(client_sock, F_GETFL, 0);
				fcntl(client_sock, F_SETFL, client_flags | O_NONBLOCK);

				struct pollfd client_pollfd;
				client_pollfd.fd = client_sock;
				client_pollfd.events = POLLIN;
				fds.push_back(client_pollfd);
				response_needed.push_back(false);
			}
		}

		for (size_t i = size; i < fds.size();) {
			if (fds[i].revents & POLLIN) {
				char buffer[1024];
				ssize_t valread = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

				if (valread > 0) {
					buffer[valread] = '\0';
					std::cout << "Recevied: " << buffer << std::endl;
					response_needed[i] = true;
					fds[i].events |= POLLOUT;
					++i;
				} else if (valread <= 0) {
					close(fds[i].fd);
					fds.erase(fds.begin() + i);
					response_needed.erase(response_needed.begin() + i);
				} else {
					++i;
				}
			} else {
				++i;
			}
		}
	}
}