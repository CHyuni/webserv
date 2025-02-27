#include "../RequestHeader/Request.hpp"

bool Request::parse(const std::string& rawRequest) {
    try {
        //1. 요청을 줄 단위로 분리
        size_t headerEnd = rawRequest.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            setError(400);
            return false;
        }

        //2. start line 파싱
        ssize_t firstLine = rawRequest.find("\r\n");
        std::string startLine = rawRequest.substr(0, firstLine);
        if (!parseStartLine(startLine)) {
            setError(400);
            return false;
        }

        //3. 헤더 파싱
        std::string headers = rawRequest.substr(firstLine + 2, headerEnd - firstLine - 2);
        if (!parseHeaders(headers)) {
            setError(400);
            return false;
        }
    } catch (const std::exception& e) {
        setError(400);
        return false;
    }
    return true;
}

bool Request::parseStartLine(const std::string& line) {
    std::istringstream iss(line);
    std::string method, url, version;

    if (!(iss >> method >> url >> version)) {
        setError(400);
        return (false);
    }

    this->_method = method;
    parseQueryString(url); // URL에서 path, query 분리
    this->_version = version;

    return true;
}

bool Request::parseHeaders(const std::string& headerSection) {
    std::istringstream iss(headerSection);
    std::string line;

    while (std::getline(iss, line)) {
        if (line.empty() || line == "\r")
            continue;

        size_t colonPos = line.find(":");
        if (colonPos == std::string::npos) {
            setError(400);
            return false;
        }
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);

        value.erase(0, value.find_first_not_of(" "));
        value.erase(value.find_last_not_of(" \r") + 1);

        this->_headers[key] = value;
    }

    //HTTP Body부의 Content-Length 처리
    if (this->_headers.count("Content-Length")) {
        try {
            std::istringstream(this->_headers.at("Content-Length")) >> this->_contentLength;
        } catch (...) {
            setError(400);
            return false;
        }
    }

    //Server name, port 멤버 변수로 넣기
    if (this->_headers.count("Host")) {
        try {
            std::string hostString;
            std::istringstream iss(this->_headers.at("Host"));
            if (!(iss >> hostString)) {
                setError(400);
                return (false);
            }
            //문자열 쪼개기
            size_t colonPos = hostString.find(":");
            if (colonPos == std::string::npos) {
                setError(400);
                return false;
            }
            this->_serverName = hostString.substr(0, colonPos);
            this->_port = hostString.substr(colonPos + 1);
        } catch (...) {
            setError(400);
            return false;
        }
    } else {
        setError(400);
        return false;
    }

    if (this->_headers.count("Cookie")) {
		try {
            std::string cookie;
            std::istringstream iss(this->_headers.at("Cookie"));
            if (!(iss >> cookie)) {
                setError(400);
                return (false);
            }
            //문자열 쪼개기
            size_t startPos = cookie.find("session_id=");
            if (startPos == std::string::npos) {
                return true;
            }
            size_t endPos =  cookie.find(";", startPos + 1);
            if (endPos == std::string::npos)
                this->_cookie = cookie.substr(startPos + 11);
            else
                this->_cookie = cookie.substr(startPos + 11, endPos - (startPos + 11));
        } catch (...) {
            setError(400);
            return false;
        }
	}

    return true;
}

bool Request::parseBody(const std::string& bodyContent) {
    if (isChunked()) {
        return parseChunkedBody(bodyContent);
    } else if (this->_contentLength > 0) {
        //일반 바디 처리
        if (bodyContent.length() != this->_contentLength) {
            setError(400);
            return false;
        }
        this->_body = bodyContent;
    }
    return true;
}

int hexToDecimal(std::string s) {
    int i;
    std::istringstream iss(s);
    iss >> std::hex >> i;
    return i;
}

bool Request::parseChunkedBody(const std::string& chunkedBody) {
	std::string s = chunkedBody;

	while(true) {
		size_t sizeEnd = s.find("\r\n");
		std::string curSizeLine = s.substr(0, sizeEnd);
		size_t curSize = hexToDecimal(curSizeLine);

		if (curSize == 0) break;
	
		s = s.substr(sizeEnd + 2);
		size_t lineEnd = s.find("\r\n");
		if (lineEnd != std::string::npos) {
			std::string chunk = s.substr(0, lineEnd);
			if (chunk.length() != curSize)
				return false;
			
			s = s.substr(lineEnd + 2);
		} else break;
	}
	return true;
}

void Request::parseQueryString(const std::string& url) {
    size_t queryPos = url.find('?');

    //Query string 처리
    if (queryPos != std::string::npos) {
        this->_path = url.substr(0, queryPos);
        this->_query = url.substr(queryPos + 1);
    } else {
        this->_path = url.substr(0);
    }
}