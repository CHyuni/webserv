#include "../RequestHeader/Request.hpp"

Request::Request() : _contentLength(0)
    , _conf(0)
    , _isParsed(false)
    , _isComplete(false)
    , _errorCode(0) {
}

Request::Request(const std::vector<std::vector<Location> > *conf) :  _contentLength(0)
    ,_conf(conf)
    , _isParsed(false)
    , _isComplete(false)
    , _errorCode(0) {
}

Request::~Request(){
}

bool Request::requestHandler(const std::string& rawRequest) {
    if (parse(rawRequest) == false) {
        if (getErrorCode() == 0)
            setError(400);    
        return false;
    }
    if (getConfigOption() == false) {
        return false;
    }
    if (validateRequest() == false) {
        return false;
    }
    _isParsed = true;
    normalizedPath();
    // debug();
    return true;
}

void Request::reset(const std::vector<std::vector<Location> > *conf) {
    this->_method = "";
    this->_path = "";
    this->_query = "";
    this->_version = "";
    this->_serverName = "";
    this->_port = "";
    this->_mappingUrl = "";
    this->_boundary = "";
    this->_headers = std::map<std::string, std::string>();
    this->_body = "";
    this->_contentLength = 0;
    this->_conf = conf;
    this->_serverBlockIdx = 0;
    this->_location.reset();
    this->_isParsed = false;
    this->_isComplete = false;
    this->_errorCode = 0;
}