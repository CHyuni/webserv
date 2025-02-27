#include "../ConfigureHeader/Location.hpp"

Location::Location(): ServerBlock(), _path(""), _cgi(), _redirect("")
{
	ServerBlock::_isServer = false;
}

Location::Location(const Location &other) : ServerBlock(other), _path(other._path), _cgi(other._cgi), _redirect(other._redirect)
{
    
}

Location& Location::operator&=(const Location &other) 
{
    if (&other != this)
    {
        ServerBlock::operator&=(other);
        _path = other._path;
        _cgi = other._cgi;
        _redirect = other._redirect;
    }
    return *this;
}

Location::~Location()
{

}

void Location::setServer(void)
{
	_isServer = true;
}


bool Location::setPath(const std::string& path)
{
    // if (!validatePath(path))
    //     return false;
    _path = path;
    return true;
}

bool Location::setCgi(const std::string& cgi)
{
    // if (!validatePath(cgi))
    //     return false;
    _cgi.push_back(cgi);
    return true;
}

bool Location::setRedirect(const std::string& redirect)
{
    // if (!validatePath(redirect))
    //     return false;
    _redirect = redirect;
    return true;
}

const std::string &Location::getPath() const
{
    return _path;
}
const std::vector<std::string> &Location::getCgi() const
{
    return _cgi;
}

const std::string &Location::getRedirect() const
{
    return _redirect;
}

void Location::reset() {
    ServerBlock::reset();
    this->_path = "";
    this->_cgi = std::vector<std::string>();
    this->_redirect = "";
}