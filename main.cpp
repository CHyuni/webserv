#include "inc/ServerSocket.hpp"
#include "inc/Webserv.hpp"
#include "inc/Location.hpp"
#include "inc/ParseConfigure.hpp"
#include "inc/Server.hpp"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "Exactly one argument is required." << std::endl;
		return 1;
	}
	try {	
		ParseConfigure configureFile;
		configureFile.splitServer(argv[1]);
		size_t size = configureFile.serverSize();
		std::vector<Server> webServer;

		for (size_t i = 0; i < size; ++i) {
			std::istringstream iss(configureFile.getServer(i));
			std::string line;
			Server temp;
			Location tmp;
			bool loca = false;
			while (getline(iss, line)) {
				if (!line.compare(0, 8, "location")) {
					Location child(temp);
					size_t pos = line.find(" ") + 1;
					child.setPath(line.substr(pos));
					loca = true;
					tmp = child;
					continue ;
				}
				if (loca && line.find("}") != std::string::npos) {
					loca = false;
					temp.setLocation(tmp.getPath(), tmp);
					continue ;
				}
				if (loca)
					ParseConfigure::sendLine(tmp, line);
				else
					ParseConfigure::sendLine(temp, line);
			}
			webServer.push_back(temp);
		}
		ServerSocket a;
		a.serverSockSet(webServer);
		a.connectClient(webServer);
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}