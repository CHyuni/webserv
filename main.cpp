#include "inc/ServerSocket.hpp"
#include "inc/Webserv.hpp"
#include "inc/Location.hpp"
#include "inc/ParseConfigure.hpp"
#include "inc/Server.hpp"

int main(int argc, char* argv[]) {
	if (argc >= 3) {
		std::cout << "Too many arguments." << std::endl;
		return 1;
	}
	std::string serverfile;
	if (argc == 1) {
		serverfile = "configure.conf";
	} else if (argc == 2) {
		serverfile = argv[1];
	}
	try {	
		ParseConfigure configureFile;
		configureFile.splitServer(serverfile);
		size_t size = configureFile.serverSize();
		std::vector<Server> webServer;

		for (size_t i = 0; i < size; ++i) {
			std::istringstream iss(configureFile.getServer(i));
			std::string line;
			Server temp;
			Location tmp;
			bool location = false;
			while (getline(iss, line)) {
				if (!line.compare(0, 8, "location")) {
					Location child(temp);
					size_t pos = line.find(" ") + 1;
					child.setPath(line.substr(pos));
					location = true;
					tmp = child;
					continue ;
				}
				if (location && line.find("}") != std::string::npos) {
					location = false;
					temp.setLocation(tmp.getPath(), tmp);
					continue ;
				}
				if (location)
					ParseConfigure::sendLine(tmp, line);
				else
					ParseConfigure::sendLine(temp, line);
			}
			webServer.push_back(temp);
			std::cout << std::endl << "ServerIndex: " << i << "\n" << "ServerName: " << std::endl;
			for (size_t j = 0; j < temp.getNameSize(); ++j) {
				std::cout << temp.getName(j) << " ";
			}
			std::cout << "\nServerPort: " << std::endl;
			for (size_t j = 0; j < temp.getPortSize(); ++j) {
				std::cout << temp.getPort(j) << " ";
			}
			std::cout << "\nServerHost: \n" << temp.getHost() << "\nServerMaxBodySize: \n" << 
			temp.getSize() << "\nServerRoot: \n" << temp.getRoot() << "\nServerMethods: \n";
			for (size_t j = 0; j < temp.getMethodsSize(); ++j) {
				std::cout << temp.getMethods(j) << " ";
			}
			std::cout << std::endl << "ServerAutoIndex: \n" << temp.getAutoidx() << "\nServerIndex: \n";
			for (size_t j = 0; j < temp.getIndexSize(); ++j) {
				std::cout << temp.getIndex(j) << " ";
			}
			std::cout << "\nServerErrorPage: \n";
			temp.getAllErrorPage();
			std::cout << "ServerLocation: \n";
			temp.getAllLocation();
		}
		ServerSocket a;
		a.serverSockSet(webServer);
		a.connectClient(webServer);
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}