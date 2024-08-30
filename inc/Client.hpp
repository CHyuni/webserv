#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Webserv.hpp"

class Client
{
private:
	size_t		_index;
	size_t		_size;
	u_int16_t	_port;
	std::string	_method;
	std::string _originUri;
	std::string	_mappingUri;
	std::string	_version;
	std::string _cliData;

public:
	Client() : _index(__SIZE_MAX__), _size(__SIZE_MAX__), _port(0), _method(""), _originUri(""), _mappingUri(""), _version(""), _cliData("") {};
	~Client() {};
	Client(const Client& rhs) : _index(rhs._index), _size(rhs._size), _port(rhs._port), _method(rhs._method), _originUri(rhs._originUri), _mappingUri(rhs._mappingUri), _version(rhs._version), _cliData(rhs._cliData) {};
	Client& operator=(const Client& rhs) {
		if (this != &rhs) {
			_index = rhs._index;
			_size = rhs._size;
			_port = rhs._port;
			_method = rhs._method;
			_originUri = rhs._originUri;
			_mappingUri = rhs._mappingUri;
			_version = rhs._version;
			_cliData = rhs._cliData;
		}
		return *this;
	}

	void		setIndex(size_t i);
	void		setSize(size_t size);
	void		setPort(u_int16_t port);
	void		setMethod(std::string method);
	void		setOriginUri(std::string uri);
	void		setMappingUri(std::string uri);
	void		setVersion(std::string version);
	void		setData(std::vector<char> data);

	size_t		getIndex();
	size_t		getSize();
	u_int16_t	getPort();
	std::string	getMethod();
	std::string	getOriginUri();
	std::string	getMappingUri();
	std::string	getVersion();
	std::string	getData();
};


#endif