#include "../RequestHeader/Request.hpp"

const std::string& Request::getMethod() const { return this->_method; }
const std::string& Request::getPath() const { return this->_path; }
const std::string& Request::getQuery() const { return this->_query; }
const std::string& Request::getVersion() const { return this->_version; }
const std::string& Request::getBody() const { return this->_body; }
const std::string& Request::getServerName() const { return this->_serverName; }
const std::string& Request::getServerPort() const { return this->_port; }
const std::map<std::string, std::string>& Request::getHeaders() const { return this->_headers; }
std::string Request::getHeader(const std::string& key) const { 
    try 
    {
        return this->_headers.at(key);
    }
    catch (const std::exception &e)
    {
        return "";
    }
}
int Request::getServerBlockIdx() const {return this->_serverBlockIdx; }
const std::vector<std::vector<Location> > *Request::getConfig() const { return this->_conf; }
const std::string& Request::getMappingUrl() const { return this->_mappingUrl; }
const std::string Request::getAccept() const { 
    if (this->_headers.count("Accept")) {
        std::string accepted = this->_headers.at("Accept");
        size_t commaPos = accepted.find(',');
        if (commaPos != std::string::npos) {
            return accepted.substr(0, commaPos);
        }
        return accepted;
    }
    return "*/*";
}

const Location& Request::getLocation() const { return this->_location; }
const std::string Request::getContentType() const {
    std::string typeHeader = getHeader("Content-Type");
    size_t pos = typeHeader.find(" ");
    std::string type = typeHeader.substr(0, pos - 1);
    return type;
}

const std::string& Request::getCookie() const { return this->_cookie; }
