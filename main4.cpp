#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>
#include <fcntl.h>
#include <signal.h>
#include <cstring>
#include <sys/epoll.h>
#include <vector>

volatile sig_atomic_t running = 1;

void signal_handler(int signum) {
    if (signum == SIGINT) {
        running = 0;
    }
}

std::string create_http_response() {
    std::ifstream file("home.html");
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

int main() {    
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    int flags = fcntl(listen_sock, F_GETFL, 0);
    fcntl(listen_sock, F_SETFL, flags | O_NONBLOCK);

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

                int flags = fcntl(clientSock, F_GETFL, 0);
                fcntl(clientSock, F_SETFL, flags | O_NONBLOCK);

                struct epoll_event clientEv;
                clientEv.events = EPOLLIN;
                clientEv.data.fd = clientSock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientSock, &clientEv) == -1) {
                    std::cerr << "epoll_ctl failed for client socket" << std::endl;
                    close(clientSock);
                    continue ;
                }
                evs[clientSock] = clientEv;
            } else if (events[i].events & EPOLLIN) {
                char buffer[8192];
                int bytes_read = recv(events[i].data.fd, buffer, sizeof(buffer), 0);

                if (bytes_read > 0) {
                    std::string request = std::string(buffer, bytes_read);
                    
                    // 요청의 첫 번째 줄과 헤더만 확인
                    std::istringstream request_stream(request);
                    std::string request_line;
                    std::getline(request_stream, request_line);  // 첫 번째 줄: GET, POST 요청 확인
                    std::cout << "Request Line: " << request_line << std::endl;

                    std::string header_line;
                    while (std::getline(request_stream, header_line) && header_line != "\r") {
                        std::cout << "Header: " << header_line << std::endl;
                    }

                    // GET 요청에 대해 HTML 페이지 응답 보내기
                    if (request_line.find("GET") != std::string::npos) {
                        std::cout << "GET request received." << std::endl;
                        std::string response = create_http_response();
                        send(events[i].data.fd, response.c_str(), response.size(), 0);
                    }
                    // POST 요청은 본문 처리하지 않고 단순히 로그만 출력
                    else if (request_line.find("POST") != std::string::npos) {
                        std::cout << "POST request received." << std::endl;
                        // POST 요청 처리 (여기서는 단순히 로그만 출력)
                    }

                    // 연결 상태 변경
                    evs[events[i].data.fd].events |= EPOLLOUT;
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &evs[events[i].data.fd]);
                } else if (bytes_read == 0) {
                    evs.erase(events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                }
            } else if (events[i].events & EPOLLOUT) {
                evs[events[i].data.fd].events &= ~EPOLLOUT;
                epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &evs[events[i].data.fd]);
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
