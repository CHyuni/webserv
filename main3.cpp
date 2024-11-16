#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>
#include <fcntl.h>
#include <signal.h>
#include <algorithm>
#include <cstring>
#include <sys/epoll.h>
#include <vector>
#include <wait.h>

volatile sig_atomic_t running = 1;

void signal_handler(int signum) {
    if (signum == SIGINT) {
        running = 0;
    }
}

std::string create_http_response() {
	std::ifstream file("html/inprogress.html");
    std::string html;
    std::string line;
    while (std::getline(file, line)) {
        html += line + "\n";
    }
    std::ostringstream ss;
    ss << html.size();
    
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + ss.str() + "\r\n";
    response += "\r\n";
    response += html;
    return response;
}

// std::string create_cookie_http_response() {
// 	std::ifstream file("html/inprogress.html");
//     std::string html;
//     std::string line;
//     while (std::getline(file, line)) {
//         html += line + "\n";
//     }
//     std::ostringstream ss;
//     ss << html.size();
    
//     std::string response = "HTTP/1.1 200 OK\r\n";
//     response += "Content-Type: text/html\r\n";
//     response += "Content-Length: " + ss.str() + "\r\n";
// 	response += "Set-Cookie: guest-id=2144; HttpOnly\r\n";
// 	response += "Set-Cookie: lang=ko; Max-Age=30; Path=/\r\n";
//     response += "\r\n";
//     response += html;
//     return response;
// }

std::string create_image_response() {
    // 바이너리 모드로 파일 열기
    std::ifstream file("favi/background.jpg", std::ios::binary);
    if (!file.is_open()) {
        return ""; // 또는 에러 응답
    }

    // 파일 크기 확인
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    // 버퍼에 파일 내용 읽기
    std::vector<char> buffer(size);
    file.read(buffer.data(), size);

    // HTTP 응답 생성
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: image/jpg\r\n";
    response += "Content-Length: " + std::to_string(size) + "\r\n";
    response += "\r\n";
    
    // 이미지 데이터 추가
    response.append(buffer.data(), size);

    return response;
}

std::string create_test_image_response() {
    // 바이너리 모드로 파일 열기
    std::ifstream file("favi/gyeongju.jpeg", std::ios::binary);
    if (!file.is_open()) {
        return ""; // 또는 에러 응답
    }

    // 파일 크기 확인
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    // 버퍼에 파일 내용 읽기
    std::vector<char> buffer(size);
    file.read(buffer.data(), size);

    // HTTP 응답 생성
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: image/jpeg\r\n";
    response += "Content-Length: " + std::to_string(size) + "\r\n";
    response += "\r\n";
    
    // 이미지 데이터 추가
    response.append(buffer.data(), size);

    return response;
}

std::string create_signUp_http_response() {
	std::ifstream file("html/signUp.html");
    std::string html;
    std::string line;
    while (std::getline(file, line)) {
        html += line + "\n";
    }
    std::ostringstream ss;
    ss << html.size();
    
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + ss.str() + "\r\n";
    response += "\r\n";
    response += html;
    return response;
}

std::string create_signIn_http_response() {
	std::ifstream file("html/signIn.html");
    std::string html;
    std::string line;
    while (std::getline(file, line)) {
        html += line + "\n";
    }
    std::ostringstream ss;
    ss << html.size();
    
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + ss.str() + "\r\n";
    response += "\r\n";
    response += html;
    return response;
}

std::string create_favi_response() {
	// 바이너리 모드로 파일 열기
    std::ifstream file("favi/favi.ico", std::ios::binary);
    if (!file.is_open()) {
        return ""; // 또는 에러 응답
    }

    // 파일 크기 확인
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    // 버퍼에 파일 내용 읽기
    std::vector<char> buffer(size);
    file.read(buffer.data(), size);

    // HTTP 응답 생성
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: image/ico\r\n";
    response += "Content-Length: " + std::to_string(size) + "\r\n";
    response += "\r\n";
    
    // 이미지 데이터 추가
    response.append(buffer.data(), size);

    return response;
}

std::string create_css_response() {
	std::ifstream file("html/styles.css");
	std::string style;
	std::string line;
	while (std::getline(file, line)) {
		style += line + "\n";
	}
	size_t content_length = style.size();

	std::ostringstream ss;
	ss << content_length;

	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/css\r\n";
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "\r\n";
	response += style;
	return response;
}

std::string dupliCgi(std::string id) {
    int fd[2];
    pid_t pid;
    const char *path = "/usr/bin/python3";
    const char* argv[] = {
        "python3",
        "/home/changhyun/42/cursus5/webserv/duplicateCheck.py",
        id.c_str(),
        NULL
    };

    if (pipe(fd) == -1)
        throw std::runtime_error("Failed to pipe");
    pid = fork();
    if (pid == -1)
    {
        close (fd[0]);
        close (fd[1]);
        throw std::runtime_error("Failed to fork");
    }
    if (pid == 0)
    {
        close (fd[0]);
        if (dup2(fd[1], 1) == -1)
        {
            close (fd[1]);
            throw std::runtime_error("Failed to fork");
        }
        close (fd[1]);
        if (execve(path, (char *const *)argv, NULL) == -1)
            throw std::runtime_error("Failed to exec");
    }
    close(fd[1]);
    char buffer[4096];
    std::string res;
    ssize_t bytes;
    while ((bytes = read(fd[0], buffer, sizeof(buffer))) > 0)
        res.append(buffer, bytes);
    close(fd[0]);
    std::cout << res << std::endl;
    int status;
    waitpid(pid, &status, 0);
    return res;
}

std::string signInCgi(std::string id, std::string pass) {
    int fd[2];
    pid_t pid;
    const char *path = "/usr/bin/python3";
    const char* argv[] = {
        "python3",
        "/home/changhyun/42/cursus5/webserv/signIn.py",
        id.c_str(),
        NULL
    };

    if (pipe(fd) == -1)
        throw std::runtime_error("Failed to pipe");
    pid = fork();
    if (pid == -1)
    {
        close (fd[0]);
        close (fd[1]);
        throw std::runtime_error("Failed to fork");
    }
    if (pid == 0)
    {
        close (fd[0]);
        if (dup2(fd[1], 1) == -1)
        {
            close (fd[1]);
            throw std::runtime_error("Failed to fork");
        }
        close (fd[1]);
        if (execve(path, (char *const *)argv, NULL) == -1)
            throw std::runtime_error("Failed to exec");
    }
    close(fd[1]);
    char buffer[4096];
    std::string res;
    ssize_t bytes;
    while ((bytes = read(fd[0], buffer, sizeof(buffer))) > 0)
        res.append(buffer, bytes);
    close(fd[0]);
    std::cout << res << std::endl;
    int status;
    waitpid(pid, &status, 0);
    return res;
}

std::string execCgi(std::string id, std::string pass) {
	int fd[2];
    pid_t pid;
	const char *path = "/usr/bin/python3";
	const char* argv[] = {
		"python3",
		"/home/changhyun/42/cursus5/webserv/signUp.py",
		id.c_str(),
		pass.c_str(),
		NULL
	};

    if (pipe(fd) == -1)
        throw std::runtime_error("Failed to pipe");
    pid = fork();
    if (pid == -1)
    {
        close (fd[0]);
        close (fd[1]);
        throw std::runtime_error("Failed to fork");
    }
    if (pid == 0)
    {
        close (fd[0]);
        if (dup2(fd[1], 1) == -1)
        {
            close (fd[1]);
            throw std::runtime_error("Failed to fork");
        }
        close (fd[1]);
        if (execve(path, (char *const *)argv, NULL) == -1)
            throw std::runtime_error("Failed to exec");
    }
    close(fd[1]);
    char buffer[4096];
    std::string res;
    ssize_t bytes;
    while ((bytes = read(fd[0], buffer, sizeof(buffer))) > 0)
        res.append(buffer, bytes);
    close(fd[0]);
    std::cout << res << std::endl;
    int status;
    waitpid(pid, &status, 0);
    return res;
}

int main() {	
	int listen_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listen_sock == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    int opt = 1;
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed" << std::endl;
        return 1;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(listen_sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    if (listen(listen_sock, SOMAXCONN) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

	int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd == -1) {
		std::cerr << "epoll_create1 failed" << std::endl;
		return 1;
	}

	std::unordered_map<int, struct epoll_event> evs;
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
		std::cerr << "epoll_ctl failed for listen socket" << std::endl;
		return 1;
	}
	evs[ev.data.fd] = ev;

	const int MAX_EVENTS = 100;
	std::vector<struct epoll_event> events(MAX_EVENTS);

	signal(SIGINT, signal_handler);
	std::unordered_map<int, std::string> result;
	while (running) {
		int event_count = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, -1);
		if (event_count == -1) {
			if (errno == EINTR) continue ;
			std::cerr << "epoll_wait failed" << std::endl;
			break ;
		}

		for (int i = 0; i < event_count; ++i) {
			if (events[i].data.fd == listen_sock) {
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int clientSock = accept(listen_sock, (struct sockaddr*)&client_addr, &client_len);
				if (clientSock < 0) {
					std::cerr << "Accept failed" << std::endl;
					continue ;
				}

				struct epoll_event clientEv;
				clientEv.events = EPOLLIN;
				clientEv.data.fd = clientSock;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientSock, &clientEv) == -1) {
					std::cerr << "epoll_ctl failed for client socket" << std::endl;
					close(clientSock);
					continue ;
				}
				evs[clientSock] = clientEv;
				continue ;
			}
			if (events[i].events & EPOLLIN) {
				char buffer[8192];
				int bytes_read = recv(events[i].data.fd, buffer, sizeof(buffer), 0);

				if (bytes_read > 0) {
					result[events[i].data.fd].append(buffer, bytes_read);
					std::cout << "Received: " << std::string(buffer, bytes_read) << std::endl;
					if (result[events[i].data.fd].size() >= 4 && result[events[i].data.fd].substr(result[events[i].data.fd].size() - 4) == "\r\n\r\n") {
						evs[events[i].data.fd].events |= EPOLLOUT;
						epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &evs[events[i].data.fd]);
					}
				} else if (bytes_read == 0) {
					evs.erase(events[i].data.fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					close(events[i].data.fd);
				} else {
				}
			}
			if (events[i].events & EPOLLOUT) {
				static std::unordered_map<int, std::string> pending_responses;
				
				if (pending_responses.find(events[i].data.fd) == pending_responses.end()) {
					if (result[events[i].data.fd].find("styles.css HTTP/1.1") != std::string::npos) {
						pending_responses[events[i].data.fd] = create_css_response();
					}
					else if (result[events[i].data.fd].find("GET /favi/background") != std::string::npos) {
						pending_responses[events[i].data.fd] = create_image_response();
					} else if (result[events[i].data.fd].find("GET /favicon.ico") != std::string::npos) {
						pending_responses[events[i].data.fd] = create_favi_response();
					} else if (result[events[i].data.fd].find("GET /favi/gyeongju") != std::string::npos) {
						pending_responses[events[i].data.fd] = create_test_image_response();
					} else if (result[events[i].data.fd].find("GET /html/signIn") != std::string::npos) {
						pending_responses[events[i].data.fd] = create_signIn_http_response();
                    }
					 else if (result[events[i].data.fd].find("GET /html/signUp") != std::string::npos) {
						pending_responses[events[i].data.fd] = create_signUp_http_response();
					} else if (result[events[i].data.fd].find("GET /signUp?") != std::string::npos) {
						size_t pos = result[events[i].data.fd].find("username=");
						size_t andpos = result[events[i].data.fd].find("&");
						std::string id = result[events[i].data.fd].substr(pos + 9, andpos - (pos + 9));
						size_t spacepos = result[events[i].data.fd].find(" ", andpos);
						std::string pass = result[events[i].data.fd].substr(andpos + 10, spacepos - (andpos + 10));
						pending_responses[events[i].data.fd] = execCgi(id, pass);
					} else if (result[events[i].data.fd].find("GET /check-username") != std::string::npos) {
                        size_t pos = result[events[i].data.fd].find("=");
                        size_t pos2 = result[events[i].data.fd].find(" ", pos);
                        std::string id = result[events[i].data.fd].substr(pos + 1, pos2 - (pos + 1));
                        pending_responses[events[i].data.fd] = dupliCgi(id);
                    } else if (result[events[i].data.fd].find("GET /signUp?") != std::string::npos) {
                        size_t pos = result[events[i].data.fd].find("username=");
						size_t andpos = result[events[i].data.fd].find("&");
						std::string id = result[events[i].data.fd].substr(pos + 9, andpos - (pos + 9));
						size_t spacepos = result[events[i].data.fd].find(" ", andpos);
						std::string pass = result[events[i].data.fd].substr(andpos + 10, spacepos - (andpos + 10));
						pending_responses[events[i].data.fd] = (id, pass);
                    }
					else {
						pending_responses[events[i].data.fd] = create_http_response();
					}
				}
				size_t to_send = std::min(pending_responses[events[i].data.fd].size(), static_cast<size_t>(8192));
				int bytes_sent = send(events[i].data.fd, pending_responses[events[i].data.fd].c_str(), to_send, 0);

				if (bytes_sent > 0) {
					pending_responses[events[i].data.fd] = pending_responses[events[i].data.fd].substr(bytes_sent);

					if (pending_responses[events[i].data.fd].empty()) {
						pending_responses.erase(events[i].data.fd);
						evs[events[i].data.fd].events &= ~EPOLLOUT;
						result.clear();
						epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &evs[events[i].data.fd]);
					}
				}
				//  else if (bytes_sent == -1) {
				// 	std::cerr << "Send failed" << std::endl;
				// }
			}
		}
	}

	for (const auto& pair : evs) {
		if (pair.first != listen_sock) {
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pair.first, NULL);
			close(pair.first);
		}
	}

	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, listen_sock, NULL);
	close(listen_sock);
	close(epoll_fd);
}
