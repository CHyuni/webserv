#include "../inc/Client.hpp"

void	Client::setIndex(size_t i) {
	_index = i;
}

void	Client::setSize(size_t size) {
	_size = size;
}

void	Client::setMethod(std::string method) {
	_method = method;
}

void	Client::setOriginUri(std::string uri) {
	_originUri = uri;
}

void	Client::setMappingUri(std::string uri) {
	_mappingUri = uri;
}

void	Client::setPort(u_int16_t port) {
	_port = port;
}

void	Client::setVersion(std::string version) {
	_version = version;
}

void	Client::setData(std::vector<char> data) {
	std::string temp(data.begin(), data.end());
	_cliData = temp;
}

size_t		Client::getIndex() {
	return _index;
}

size_t		Client::getSize() {
	return _size;
}

u_int16_t	Client::getPort() {
	return _port;
}

std::string	Client::getMethod() {
	return _method;
}

std::string Client::getOriginUri() {
	return _originUri;
}

std::string	Client::getMappingUri() {
	return _mappingUri;
}

std::string	Client::getVersion() {
	return _version;
}

std::string	Client::getData() {
	return _cliData;
}