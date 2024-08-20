#include "../inc/ParseConfigure.hpp"

const std::string ParseConfigure::validKey[] = {
		"listen", "server_name", "host", "error_page", "client_max_body_size", 
		"location", "root", "index", "allow_methods", "autoindex", ""
};

void ParseConfigure::splitServer(const std::string& rhs) {
	std::ifstream	file(rhs.c_str());
	std::string		temp, line;
	int				left = 0, right = 0;

	if (!file.is_open()) {
		std::cout << "openError\n";
		return ; // << throw expected
	}
	while (getline(file, line)) {
		std::string::iterator it = line.begin();
		while (it != line.end() && std::isspace(*it)) {
			it = line.erase(it);
		}
		if (line[0] == '#' || line.empty())
			continue;
		if(!checkLine(line)) {
			return ; // << throw expected
		}
		if (line.find("{") != std::string::npos)
			left++;
		else if (line.find("}") != std::string::npos)
			right++;
		temp += line + "\n";
		if ((left != 0 && right != 0) && left == right) {
			_server.push_back(temp);
			temp.clear();
			left = 0;
			right = 0;
		}
	}
	if (left != right) {
		return ; // << throw expected
	}
}

bool ParseConfigure::checkLine(const std::string& rhs) {
	for (int i = 0; !validKey[i].empty(); ++i) {
		if (rhs.compare(0, validKey[i].length(), validKey[i]) == 0)
			return true;
	}
	return (rhs == "{" || rhs == "}" || rhs == "server {");
}

std::string ParseConfigure::lineGet(const std::string& rhs) {
	for (int i = 0; !validKey[i].empty(); ++i) {
		if (rhs.compare(0, validKey[i].length(), validKey[i]) == 0) {
			size_t idx = validKey[i].length();
			return rhs.substr(idx);
		}
	}
	if (rhs.find("server {") != std::string::npos)
		return "";
	return "error"; // << throw expected
}

std::string ParseConfigure::getServer(const size_t& i) {
	return _server[i];
}

size_t ParseConfigure::serverSize() {
	return _server.size();
}