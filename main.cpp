#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

const int PORT = 8080;
int server_fd = -1;

void    signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << std::endl << "Process exit." << std::endl;
        if (server_fd != -1) {
            close(server_fd);
        }
        exit(0);
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
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    signal(SIGINT, signal_handler);

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    // int opt = 1;
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    //     std::cerr << "setsockopt failed" << std::endl;
    //     return 1;
    // }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓에 주소 할당
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    // 연결 대기
    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while(true) {
        // 클라이언트 연결 수락
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        // 클라이언트로부터 데이터 수신
        int valread = read(new_socket, buffer, 1024);
        std::cout << "Received request" << std::endl;

        // HTTP 응답 생성 및 전송
        if (valread > 0) {
            std::string request(buffer, valread);

            if (request.find("GET") != std::string::npos) {
                std::cout << "ok" << std::endl;
            } else {
                std::cout << "false" << std::endl;
            }

            std::cout << request << std::endl;
        }
        std::string response = create_http_response();
        send(new_socket, response.c_str(), response.length(), 0);
        std::cout << "Response sent" << std::endl;

        close(new_socket);
    }

    return 0;
}