#include "../inc/ParseConfigure.hpp"
#include "../inc/Location.hpp"
#include "../inc/Server.hpp"

const std::string ParseConfigure::validKey[] = {
		"listen", "server_name", "host", "error_page", "client_max_body_size", 
		"location", "root", "index", "allow_methods", "autoindex", ""
};

void ParseConfigure::splitServer(const std::string& rhs) {
	std::ifstream	file(rhs.c_str());
	std::string		temp, line;
	int				left = 0, right = 0, cnt = 0;
	bool			locationCheck = false;
	bool			locationCheck2 = false;

	if (!file.is_open()) {
		std::runtime_error("File open Error.");
	}
	while (getline(file, line)) {
		std::string::iterator it = line.begin();
		while (it != line.end() && std::isspace(*it)) {
			it = line.erase(it);
		}
		if (line[0] == '#' || line.empty())
			continue;
		if(!checkLine(line, locationCheck, locationCheck2)) {
			throw std::invalid_argument("Invalid configuer");
		}
		if (locationCheck && !locationCheck2)
				cnt++;
		if ((line.find("{") != std::string::npos)) {
			left++;
		}
		if (line.find("}") != std::string::npos) {
			if (locationCheck && locationCheck2) {
				locationCheck = false;
				locationCheck2 = false;
				cnt = 0;
			}
			right++;
		}
		if (cnt > 1)
			throw std::invalid_argument("Invalid configure");
		temp += line + "\n";
		if ((left != 0 && right != 0) && left == right) {
			_server.push_back(temp);
			temp.clear();
			left = 0;
			right = 0;
		}
	}
	if (left != right) {
		throw std::invalid_argument("Invalid configure");
	}
}

bool ParseConfigure::checkLine(const std::string& rhs, bool& loca, bool& loca2) {
	for (int i = 0; !validKey[i].empty(); ++i) {
		if (rhs.compare(0, validKey[i].length(), validKey[i]) == 0) {
			if (validKey[i] == "location") {
				loca = true;
			}
			if (loca && rhs.find("{") != std::string::npos)
						loca2 = true;
			return true;
		}
	}
	if (loca && rhs.find("{") != std::string::npos)
						loca2 = true;
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
	throw std::invalid_argument("Invalid configure");
}

std::string ParseConfigure::getServer(const size_t& i) {
	return _server[i];
}

size_t ParseConfigure::serverSize() {
	return _server.size();
}

void ParseConfigure::sendLine(Server& web, const std::string& str) {
	size_t pos = 0;
	while (pos < str.size() && !isspace(str[pos]))
		++pos;
	if (!str.compare(0, 11, "server_name"))
		web.setName(str.substr(pos));
	else if (!str.compare(0, 6, "listen"))
		web.setPort(str.substr(pos));
	else if (!str.compare(0, 4, "host"))
		web.setHost(str.substr(pos));
	else if (!str.compare(0, 20, "client_max_body_size"))
		web.setSize(str.substr(pos));
	else if (!str.compare(0, 4, "root"))
		web.setRoot(str.substr(pos));
	else if (!str.compare(0, 13, "allow_methods"))
		web.setMethods(str.substr(pos));
	else if (!str.compare(0, 9, "autoindex"))
		web.setAutoidx(str.substr(pos));
	else if (!str.compare(0, 5, "index"))
		web.setIndex(str.substr(pos));
	else if (!str.compare(0, 10, "error_page"))
		web.setError(str.substr(pos));
	else if (!str.compare(0, 6, "server")) {
		return ;
	}
}