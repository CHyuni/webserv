#include "../ConfigureHeader/ServerBlock.hpp"

ServerBlock::ServerBlock() : _host(""), _port(), _server(), _size(0),
 _root(""), _methods(), _autoidx(false), _index(), _error(), _isServer(true), _isAuto(false)
{
    _error["400"] = "ErrorHtml/400.html";
    _error["401"] = "ErrorHtml/401.html";
    _error["402"] = "ErrorHtml/402.html";
    _error["403"] = "ErrorHtml/403.html";
    _error["404"] = "ErrorHtml/404.html";
    _error["405"] = "ErrorHtml/405.html";
    _error["413"] = "ErrorHtml/413.html";
    _error["500"] = "ErrorHtml/500.html";
    _error["501"] = "ErrorHtml/501.html";
}

ServerBlock::ServerBlock(const ServerBlock &other): _host(other._host), _port(other._port), _server(other._server), _size(other._size),
 _root(other._root), _methods(other._methods), _autoidx(other._autoidx), _index(other._index), _error(other._error), _errormap(other._errormap),
 _isServer(other._isServer), _isAuto(other._isAuto),_alias(other._alias)
{
}

ServerBlock &ServerBlock::operator&=(const ServerBlock &other)
{
    if (&other != this)
    {
        _host = other._host;
        _port = other._port;
        _server = other._server;
        _size = other._size;
        _root = other._root;
        _methods = other._methods;
        _autoidx = other._autoidx;
        _index = other._index;
        _error = other._error;
		_isServer = other._isServer;
		_isAuto = other._isAuto;
        _alias = other._alias;
        _errormap = other._errormap;
    }
    return *this;
}

ServerBlock::~ServerBlock()
{
}

void ServerBlock::setMaxBodySize(const unsigned int &value)
{
    _size = value ;
}


unsigned int ServerBlock::strtoul(const std::string &value)
{
    char *end;

    unsigned long result = std::strtoul(value.c_str(), &end, 10);
    
    if (*end != '\0')
        return MAX_BODY_SIZE;
    return result;
}
bool ServerBlock::setHost(const std::string& host)
{
    if (host.empty() || host[0] == '.' || _host != "")
        return false;
    int count = 0;
    int dot = 0;
    std::string::const_iterator start = host.begin();
    
    for (std::string::const_iterator i = host.begin(); i != host.end(); ++i)
    {
        if (!isdigit(*i) && *i != '.')
            return false;
        if (*i == '.')
        {
            if ((*(i - 1) && *(i - 1) == '.') || count > 3 
            || (count == 2 && *(i - 2) == '0') || (count == 3 && *(i - 3) == '0')
            || strtoul(std::string(start, i)) > 255)
                return false;
            count = -1;
            ++dot;
            start = i + 1;
        }
        ++count;
    }
    if (dot != 3 || start == host.end())
        return false;
    _host = host;
    return true;
}

bool ServerBlock::setPort(unsigned int port)
{
    if (std::find(_port.begin(), _port.end(), port) != _port.end() || port > 65535 || port < 1024)
        return false;  
    _port.push_back(port);
    return true;
}
bool ServerBlock::setServerName(const std::string& server) 
{
	if (server.empty())
		return false;
    if (std::find(_server.begin(), _server.end(), server) != _server.end())
        return false;
    _server.push_back(server);
    return true;
}
bool ServerBlock::setClientMaxBodySize(const std::string &value) 
{
    char *end;

	if (_size) {
		return false;
    }
    unsigned long size = std::strtoul(value.c_str(), &end, 10);
    if (*end == 'M')
        size *= 1024 * 1024;
    else if (*end == 'K')
        size *= 1024;
    if (size == 0 || size > MAX_BODY_SIZE || (*end && *(end + 1))) {
    // if (size == 0 || (*end && *(end + 1))) {
        std::cout << "this!\n" << size << " " << MAX_BODY_SIZE << std::endl;
        return false;
    }
    _size = size;
    return true;
}

bool ServerBlock::validatePath(const std::string& path)
{
    if (path.empty() || path[0] != '/')
        return false;
    // if (access(path.c_str(), X_OK) != 0)
    //     return false;
   for (size_t i = 0; i < path.length(); i++)
   {
       char c = path[i];
       if ((!isalnum(c) && c != '.' && c != '/' && c != '-' && c != '+' && c != '_') || 
       (i > 1 && c == '/' && path[i - 1] == '/'))
           return false;
   }
   return true;
}

bool ServerBlock::setErrorPage(const std::string& num, const std::string& page)
{    
    unsigned int numInt = strtoul(num);
    
    if (numInt < 400 || numInt > 507 || !validatePath(page))
        return false;
    _error[num] = page;
    _errormap.push_back(num);
    std::cout << "asd" << _errormap.size() <<std::endl;
    // _error.insert(std::make_pair(num, page));
    return true;
}

bool ServerBlock::setRoot(const std::string& root)
{
    if ( _root != "" || !validatePath(root))
        return false;
    _root = root;
    return true;
}
bool ServerBlock::setIndex(const std::string& index)
{
    // if (!_root.empty() && _root[_root.size() - 1] != '/')
    //     _root += '/';
    // if (access((_root + index).c_str(), X_OK) != 0)
    //     return false;
	if (index.empty())
		return false;
	if (std::find(_index.begin(), _index.end(), index) != _index.end())
        return false; 
    _index.push_back(index);
    return true;
}
bool ServerBlock::setMethods(const std::string& methods)
{
    if (!(methods == "GET" || methods == "POST" || methods == "DELETE"|| methods == "PUT"))
        return false;
    if (std::find(_methods.begin(), _methods.end(), methods) != _methods.end())
        return false; 
    _methods.push_back(methods);
    return true;
}
bool ServerBlock::setAutoindex(const std::string& autoindex)
{
    if (_isAuto || (autoindex != "on" && autoindex != "off"))
        return false;
    _isAuto = 1;
    if (autoindex == "on")
        _autoidx = true;
    return true;
}
const std::string &ServerBlock::getHost() const
{
    return _host;
}
const std::vector<unsigned int> &ServerBlock::getPort() const
{
    return _port;
}
const std::vector<std::string> &ServerBlock::getServerName() const
{
    return _server;
}
const unsigned int &ServerBlock::getClientMaxBodySize() const
{
    return _size;
}
const std::map<std::string, std::string> &ServerBlock::getErrorPage() const
{
    return _error;
}
const std::string &ServerBlock::getRoot() const
{
    return _root;
}
const std::vector<std::string> &ServerBlock::getIndex() const
{
    return _index;
}
const std::vector<std::string> &ServerBlock::getMethods() const
{
    return _methods;
}
const bool &ServerBlock::getAutoindex() const
{
    return _autoidx;
}
const bool &ServerBlock::getIsServer() const
{
    return _isServer;
}

const std::vector<std::string> &ServerBlock::getErrmap() const
{
    return _errormap;
}

void ServerBlock::reset() {
    this->_host = "";
    this->_port = std::vector<unsigned int>();
    this->_server = std::vector<std::string>();
    this->_size = 0;
    this->_root = "";
    this->_methods = std::vector<std::string>();
    this->_autoidx = false;
	this->_index = std::vector<std::string>();
    this->_error = std::map<std::string, std::string>();
    this->_isServer = 0;
    this->_isAuto = 0;
}