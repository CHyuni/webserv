#include "../RequestHeader/Request.hpp"

bool Request::isChunked() const {
    std::string chunked;

    if (this->_headers.count("Transfer-Encoding")) {
        chunked = this->_headers.at("Transfer-Encoding");
        if (chunked == "chunked") {
            return true;
        }    
    }
    return false;
}

bool Request::isComplete() const {
    return this->_isComplete;
}

bool Request::hasError() const {
    if (this->_errorCode == 0)
        return false;
    return true;
}

int Request::getErrorCode() const {
    return this->_errorCode;
}

size_t Request::getContentLength() const {
    return this->_contentLength;
}
