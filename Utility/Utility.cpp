#include "../UtilityHeader/Utility.hpp"

bool isDirectory(const std::string& path) {
	struct stat info;

	if (!stat(path.c_str(), &info)) {
		if (S_ISDIR(info.st_mode)) {
			return true;
		}
	}
	return false;
}

int checkPermissions(const std::string& path) {
	std::cout << "path:" <<path <<",access:"<<access(path.c_str(), F_OK)<< std::endl;
	if (!access(path.c_str(), F_OK)) {
		if (!access(path.c_str(), R_OK)) {
			return 0;	
		}
		else return 403;
	}
	return 404;
}

std::string setNormalizedPath(const std::string& _path, const std::string& _root) {
    std::string path, temp;

    for (size_t i = 0, j = 0; i < _path.size(); ++i) {
        if (i && (temp[j] == '/' && _path[i] == '/')) continue;
        temp += _path[i];
        j++;
    }

	path = _root + temp;
	if (isDirectory(path)) {
		
	}
	return path;
}

std::string getGMTDate() {
	time_t now = time(0);
	struct tm *gmtm = gmtime(&now);
	char date_str[100];
	strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", gmtm);
	return std::string(date_str);
}

void getArgv(std::vector<std::string>& argv, const std::string& str) {
	size_t size = 0;
	size_t pos = str.find('=');
	while (pos != std::string::npos) {
		size++;
		pos = str.find('=', pos + 1);
	}
	std::string temp = str;
	for (size_t i = 0; i < size; ++i) {
		size_t start = temp.find('=');
		size_t endpos;
		endpos = temp.find('&', start);
		if (endpos != std::string::npos) {
			argv.push_back((temp.substr(start + 1, endpos - start - 1)));
			temp = temp.substr(endpos + 1);
		}
		else {
			argv.push_back(temp.substr(start + 1));
		}
	}
}

std::string createContentLength(const std::string& str) {
	size_t size = str.length();
	std::string result = "Content-Length: " + ToString(size) + "\r\n";
	return result;
}

bool checkDownload(const std::string& path) {
	size_t pos = path.find(".");
	if (pos == std::string::npos) return true;
	std::string temp = path.substr(pos + 1);
	std::vector<std::string> check;
	check.push_back("exe");
	check.push_back("bin");
	check.push_back("pdf");
	check.push_back("zip");
	for (size_t i = 0; i < 4; ++i) {
		if (temp == check[i]) return true;
	}
	return false;
}

std::string reGetAccept(const std::string& type) {
	std::map<std::string, std::string> map;
	map["html"] = "text/html";
	map["htm"] = "text/html";
	map["css"] = "text/css";
	map["json"] = "application/json";
	map["jpeg"] = "image/jpeg";
	map["jpg"] = "image/jpeg";
	map["png"] = "image/png";

	std::map<std::string, std::string>::iterator it = map.find(type);
	if (it != map.end())
		return it->second;
	return "text/plain";
}

// URL 디코딩 함수
std::string urlDecode(const std::string& str) {
    std::string result;
    char ch;
    int hexValue;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%') {
            if (i + 2 < str.length() &&
                std::isxdigit(str[i + 1]) &&
                std::isxdigit(str[i + 2])) {
                std::istringstream iss(str.substr(i + 1, 2));
                iss >> std::hex >> hexValue;
                ch = static_cast<char>(hexValue);
                result += ch;
                i += 2;
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}