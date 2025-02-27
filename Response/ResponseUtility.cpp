#include "../ResponseHeader/Response.hpp"

void responseNormalized(std::string& response) {
	size_t pos = response.find("HTTP");
	if (pos == std::string::npos) {
		pos = response.find("Status");
		if (pos != std::string::npos) {
			response.erase(response.begin() + pos, response.begin() + pos + 7);
			response.insert(0, "HTTP/1.1");
		}
	}

    if (response.find("Content-Length") == std::string::npos) {
		size_t size = 4;
        size_t postmp = response.find("\r\n\r\n");
        if (postmp == std::string::npos) {
            postmp = response.find("\r\n");
			size = 2;
		}
		if (postmp != std::string::npos) {
			size_t length = response.substr(postmp + size).length();
			response.insert(postmp, "Content-Length: " + ToString(length) + "\r\n\r\n");
		}
    }
}