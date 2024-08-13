#include <arpa/inet.h>
#include <iostream>
#include <string>

int main() {
	uint16_t a = 8080;
	uint16_t b = htons(a);
	uint16_t c = ntohs(b);

	struct sockaddr_in address;
	struct sockaddr_in address2;
	std::string add = "127.0.0.1";
	inet_pton(AF_INET, add.c_str(), &address.sin_addr);
	uint32_t d = ntohl(address.sin_addr.s_addr);
	address2.sin_addr.s_addr = inet_addr(add.c_str());
	uint32_t f = ntohl(address.sin_addr.s_addr);
	char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &address.sin_addr, str, INET_ADDRSTRLEN);


	std::cout << a << " " << b << " " << c << " " << add << " " 
	<< address.sin_addr.s_addr << " " << d <<  " " << str 
	<< address2.sin_addr.s_addr << " " << f << std::endl;
}