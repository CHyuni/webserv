#include "../RequestHeader/Request.hpp"

void Request::setError(int code) {
    this->_errorCode = code;
}

void Request::normalizedPath() { // 경로 정규화 (../와 ./ 처리)
    std::string temp;

    for (size_t i = 0, j = 0; i < _path.size(); ++i) {
        if (i && (temp[j] == '/' && _path[i] == '/')) continue;
        temp += _path[i];
        j++;
    }
    if (_location.getAlias().empty())
        _mappingUrl = _location.getRoot() + temp;
    else {
        size_t pos = getPath().rfind("/");
        if (!pos)
            _mappingUrl = _location.getAlias();
        else {
            pos = getPath().find("/", 1);
            _mappingUrl = _location.getAlias() + getPath().substr(pos);
        }
    }
}

bool Request::validateRequest() {
    int methodFindIdx = -1;
    for(size_t methodIdx = 0; methodIdx < _location.getMethods().size(); methodIdx++) {
        if (this->getMethod() == _location.getMethods()[methodIdx]) {
            methodFindIdx = methodIdx;
        }
    }
    if (methodFindIdx == -1) {
        setError(405);
        return false;
    }
    return true;
}

void Request::setMappingUrl(std::string& path) {
    this->_mappingUrl = path;
}

void Request::setBody(const std::string& body) {
    this->_body = body;
}

void Request::setChunkedBody(const std::string& body) {
    size_t pos = 0;
    while (true) {
        size_t crlf = body.find("\r\n", pos);
        if (crlf == std::string::npos) break;
        std::string chunk_size_str = body.substr(pos, crlf - pos);
        char* end;
        long chunk_size = strtol(chunk_size_str.c_str(), &end, 16);
        if (chunk_size == 0)
        {
            break;
        }
        size_t chunk_start = crlf + 2;
        size_t chunk_end = chunk_start + chunk_size;
        if (chunk_end <= body.length()) {
            this->_body.append(body.substr(chunk_start, chunk_size));
        } else {
            break;
        }
        pos = chunk_end + 2;
    }
    this->_contentLength = _body.length();
}