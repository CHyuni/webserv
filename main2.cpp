#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <cstddef>
#include <vector>

const int PORT = 8080;
int server_fd = -1;
std::vector<struct pollfd> fds;

void    signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << std::endl << "Process exit." << std::endl;
        if (server_fd != -1) {
            close(server_fd);
        }
        exit(0);
    }
}

std::string faviCreate() {
    std::string html = R"(
<!DOCTYPE html>
<html>
<head>
  <title>My Page</title>
  <link rel="icon" href="favi/favi.ico" type="image/x-icon">
</head>
<body>
  <h1>Welcome to my page!</h1>
</body>
</html>
)";
return html;
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
    address.sin_port = htons(PORT);

    if (bind(listen_sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    if (listen(listen_sock, SOMAXCONN) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    struct pollfd listen_pollfd;
    listen_pollfd.fd = listen_sock;
    listen_pollfd.events = POLLIN;
    fds.push_back(listen_pollfd);

    signal(SIGINT, signal_handler);

    std::vector<bool> response_needed(fds.size(), false);
    int a = 0;
    while(true) {
        int ret = poll(&fds[0], fds.size(), -1);
        if (ret < 0) {
            std::cerr << "poll failed" << std::endl;
            break;
        }
        if (fds[0].revents & POLLIN) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &client_len);
            if (client_sock > 0) {
                int client_flags = fcntl(client_sock, F_GETFL, 0);
                fcntl(client_sock, F_SETFL, client_flags | O_NONBLOCK);

                struct pollfd client_pollfd;
                client_pollfd.fd = client_sock;
                client_pollfd.events = POLLIN;
                fds.push_back(client_pollfd);
                response_needed.push_back(false);
            }
        }

        for (size_t i = 1; i < fds.size(); i++) {
            if (fds[i].revents & POLLIN) {
                std::vector<char> buffer(8192);
                std::vector<char> totalData;
                ssize_t valread = recv(fds[i].fd, &buffer[0], buffer.size(), 0);

                if (valread > 0) {
                    totalData.insert(totalData.end(), buffer.begin(), buffer.begin() + valread);
                    std::cout << "Received\n" << &totalData[0] << std::endl;
                    response_needed[i] = true;
                    fds[i].events |= POLLOUT;
                    // std::string temp;
                    //     temp.assign(buffer);
                    //     if (temp.find("GET / HTTP/1.1") != std::string::npos) {
                    //         std::cout << "erwqeqw\n";
                    //         a = 1;
                    //     }
                } else if (valread <= 0) {
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        response_needed.erase(response_needed.begin() + i);
                        --i;
                }
            }

            if (fds[i].revents & POLLOUT && response_needed[i]) {
                std::string response = create_http_response();
                ssize_t valsend = send(fds[i].fd, response.c_str(), response.length(), 0);
                if (valsend > 0) {
                    std::cout << "Response sent" << std::endl;
                    response_needed[i] = false;
                    fds[i].events &= ~POLLOUT;
                } else if (valsend <= 0) {
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    response_needed.erase(response_needed.begin() + i);
                    continue;
                }
            }

            if (fds[i].revents & (POLLERR | POLLHUP)) {
                if (POLLERR)
                    std::cout << "Error" << std::endl;
                else
                    std::cout << "Connection closed" << std::endl;
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                response_needed.erase(response_needed.begin() + i);
                --i;
            }
        }
    }
    for (size_t i = 0; i < fds.size(); ++i) {
        close(fds[i].fd);
    }
}
