#include "inc/Webserv.hpp"
#include "inc/Location.hpp"
#include "inc/ParseConfigure.hpp"
#include "inc/Server.hpp"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "Exactly one argument is required." << std::endl;
		return 1;
	}
	std::ifstream file(argv[1]);
	if (!file.is_open()) {
		std::cout << "Failed to open file: " <<  argv[1] << std::endl;
		return 1;
	}
	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::string content;
	content.reserve(fileSize);

	std::string line;
	while (std::getline(file, line)) {
		content += line + '\n';
	}
	
	ParseConfigure a;
	a.splitServer(argv[1]);
	size_t size = a.serverSize();

	std::cout << a.getServer(size - 1) << std::endl;
}