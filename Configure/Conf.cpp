#include "../ConfigureHeader/Conf.hpp"

class Conf::InputErrException : public std::exception
{
    private :
        ErrorType _type;
    public :
        InputErrException(ErrorType type) : _type(type) {};
    	const char* what() const throw()
        {
            switch (_type)
            {
                case TOO_MANY_ARGS:
                    return "Too many arguments";
                case FILE_NOT_FOUND:
                    return "File not found";
                case INVALID_FORMAT:
                    return "Invalid format";
                default:
                    return "Unknown error";
            }
        }
};

char* Conf::checkInput(const int &args, char* &argv)
{
    const char *res = "default.conf";
    
    if (args == 1)
        return const_cast<char *>(res);
	else if (args == 2)
        return argv;
	throw Conf::InputErrException(TOO_MANY_ARGS);
}

Conf::Conf(const std::string &file) : _file(file.c_str()), _line("")
{
    if (!_file.is_open())
        throw InputErrException(FILE_NOT_FOUND);
    _confMap["host"] = HOST;
    _confMap["listen"] = LISTEN;
    _confMap["server_name"] = SERVER_NAME;
    _confMap["client_max_body_size"] = CLIENT_MAX_BODY_SIZE;
    _confMap["error_page"] = ERROR_PAGE;
    _confMap["location"] = LOCATION;
    _confMap["root"] = ROOT;
    _confMap["index"] = INDEX;
    _confMap["methods"] = METHODS;
    _confMap["autoindex"] = AUTOINDEX;
    _confMap["path"] = PATH;
    _confMap["cgi_path"] = CGI_PATH;
    _confMap["redirect"] = REDIRECT;
    _confMap["alias"] = ALIAS;
}

Conf::Conf(const Conf &other) : _line(other._line)
{
}

Conf &Conf::operator&=(const Conf &other)
{
    if (&other != this)
    { 
        _line = other._line; 
    }
    return *this;
}

Conf::~Conf()
{

}

void Conf::makeLine()
{
    std::string line;

    while(std::getline(_file, line))
    {
        _line += line + '\n';
    }
    _line.erase(_line.size() - 1);
}

bool Conf::checkBrace()
{
    std::stack<char> checkStack;
    const char* validChars = "{}/_.- ;+\n\t: ";

    for (std::string::const_iterator i = _line.begin(); i != _line.end(); ++i)
    {
        if (!isalnum(*i) && !strchr(validChars, *i))
            return false;
        if (*i == '{')
            checkStack.push('{');
        else if (*i == '}')
        {
            if (checkStack.size() == 0)
                return false;
            checkStack.pop();
        }
    }
    return (checkStack.size() == 0);
}


bool    Conf::isSpace(std::string::const_iterator &i)
{
    return *i == ' ' || *i == '\t' || *i == '\n';
}

bool    Conf::isFirstLine(std::string::const_iterator &i)
{
    const char *server = "server";
    
    if (strncmp(server, &(*i), 6))
        return false;
    i += 6;
    while (*i && isSpace(i))
        ++i;
    return (*i == '{');
}

bool    Conf::allowConf(const std::string &conf)
{
    return _confMap.find(conf) != _confMap.end();
}

bool    Conf::parseErrorpage(const std::string &value, std::string::const_iterator &i)
{
    std::string val = value;

    while (*i != ';')
    {
        std::string::const_iterator start;
        if (val.empty())
        {
            while (*i && isSpace(i)) {++i;}
            start = i;
            while (*i && *i != ';' && !isSpace(i)) {++i;}
            val = std::string(start, (i));
        }
        while (*i && isSpace(i)) {++i;}
        if (!*i)
            return false;
        start = i;
        while (*i && *i != ';' && !isSpace(i)) {++i;}
        if (!_back->setErrorPage(val, std::string(start, (i))))
            return false;
        val = "";
    }
    return true;
}

bool    Conf::switchCase(const std::string &key, const std::string &val)
{
    switch (_confMap[key])
    {
        case HOST:
            return _back->setHost(val);
        case LISTEN:
            return _back->setPort(ServerBlock::strtoul(val));
        case SERVER_NAME:
            return _back->setServerName(val);
        case CLIENT_MAX_BODY_SIZE:
            return _back->setClientMaxBodySize(val);
        case ROOT:
            return _back->setRoot(val);
        case INDEX:
            return _back->setIndex(val);
        case METHODS:
            return _back->setMethods(val);
        case AUTOINDEX:
            return _back->setAutoindex(val);
        case CGI_PATH:
            return _back->setCgi(val);
        case REDIRECT:
            return _back->setRedirect(val);
        case ALIAS:
        {
            return _back->setAlias(val);
        }
        default :
            return false;
    }
}

bool    Conf::parseCommon(const std::string &key, const std::string &value, std::string::const_iterator &i)
{
    std::string val = value;
    while (*i != ';')
    {
        if (val.empty())
        {
            std::string::const_iterator start;
            
            while (*i && isSpace(i)) {++i;}
            start = i;
            while (*i && *i != ';' && !isSpace(i)) {++i;}
            val = std::string(start, (i));
        }
        if (*i != ';')
        {
            if (!switchCase(key, val)) {
                return false;
            }
        val = "";
        }
    }
    return switchCase(key, val);
}

bool    Conf::putBlock(const std::string &key, const std::string &value, std::string::const_iterator &i)
{
    if (_confMap[key] != ERROR_PAGE) {
        return parseCommon(key, value, i);
    }
    return parseErrorpage(value, i);
}

bool    Conf::parseLoc(std::string &key, std::string::const_iterator &start, std::string::const_iterator &i)
{
    if (key == "location")
    {
        _block.push_back(Location());
        _back = &_block.back();
        while (*i && isSpace(i)) {++i;}
        start = i;
        while (*i && !isSpace(i) && *i != '{') {++i;}
        if (!_back->setPath(std::string(start, i)))
            return false;
        ++i;
        if (!parseLine(i))
            return false;
        return true;
    }
    return false;
}

bool    Conf::parseLine(std::string::const_iterator &i)
{
    if (*i == '{')
        ++i;
    while (*i && isSpace(i)) {++i;}
    if (*i == '}')
        return true;
    std::string key;
    std::string::const_iterator start = i;

    while (*i && !isSpace(i)) {++i;}
    key = std::string(start, i);
	// std::cout << "key: " << key << std::endl;
    while (*i && isSpace(i)) {++i;}
    if (key.empty())
        return true;
    else if (*i == '{')
    {
        _block.push_back(Location());
        _back = &_block.back();
        return true;
    }
    if (!allowConf(key)) {
        return false;
    }
    if (parseLoc(key, start, i))
        return true;
    while (*i && isSpace(i)) {++i;}
    start = i;
    while (*i && *i != ';' && !isspace(*i)) {++i;}
    if (!putBlock(key, std::string(start, i), i)) {
        return false;
    }
    return *(i++) == ';';
}

bool    Conf::parseBlock(std::string::const_iterator &i)
{
    if (!isFirstLine(i)) {
        return false;
    }
    else
    {
        _block.push_back(Location());
        _back = &_block.back();
		_back->setServer();
    }
    while (!isFirstLine(i) && *i)
    {
        if (!parseLine(i)) {
            return false;
        }
        if (*i)
            ++i;
    }
    if (*i != '\0')
    {
        --i;
        while ((*i == ' ' || *i == '\t' || *i == '\n') && *i)
            --i;
        i -= 6;
    }
    return true;
}

void Conf::parseConf()
{
    makeLine();
    if (!checkBrace()) {
        throw Conf::InputErrException(INVALID_FORMAT);
    }
    std::string::const_iterator i = _line.begin();
    while (i != _line.end())
    {
        if (!parseBlock(i)) {
            throw Conf::InputErrException(INVALID_FORMAT);
        }
        if (*i)
            ++i;
    }
    // std::cout << "22"<<_server[0][5].getAutoindex() << std::endl;
    // std::cout << _server[0][5].getAlias() << std::endl;



    organizeServerBlocks();
    printConfig();

}

void Conf::updateLoc(Location &loc)
{

    Location &firstLoc = _server.back().front();
    
    if (_server.back().front().getHost().empty())
        throw Conf::InputErrException(INVALID_FORMAT);
    if (loc.getHost().empty())
    {
        if (firstLoc.getHost().empty())
            throw std::runtime_error("empty host");            
        loc.setHost(firstLoc.getHost());
    } 
    if (loc.getPort().empty())
    {
        if (_server.back().front().getPort().empty())
            throw Conf::InputErrException(INVALID_FORMAT);
        std::vector<unsigned int> tmp = firstLoc.getPort();
        if (tmp.empty())
                throw std::runtime_error("empty port");
        for (unsigned int i = 0; i != tmp.size(); ++i)
        {
            loc.setPort(tmp[i]);
        }
    }
    if (loc.getServerName().empty())
    {
        std::vector<std::string> tmp = firstLoc.getServerName();
        if (tmp.empty())
                throw std::runtime_error("empty servername");
        for (unsigned int i = 0; i != tmp.size(); ++i)
        {
            loc.setServerName(tmp[i]);
        }
    }
    if (loc.getClientMaxBodySize() == 0)
    {
        std::stringstream ss;
        ss << firstLoc.getClientMaxBodySize();
        if (ss.str() == "0")
            loc.setMaxBodySize(1000000000);
        else
            loc.setClientMaxBodySize(ss.str());
    }
    if (loc.getRoot().empty() && loc.getAlias().empty())
    {
        if (firstLoc.getRoot().empty() && firstLoc.getAlias().empty())
            throw std::runtime_error("empty root or alias");
        loc.setRoot(firstLoc.getRoot());
    }
    if (loc.getMethods().empty())
    {
        std::vector<std::string> tmp = firstLoc.getMethods();
        if (tmp.empty())
            throw std::runtime_error("empty method");
        for (unsigned int i = 0; i != tmp.size(); ++i)
        {
            loc.setMethods(tmp[i]);
        }
    }
    if (loc.getIndex().empty())
    {
        // if (firstLoc.getIndex().empty())
        // {
        //     loc.setIndex("index.html");
        //     return ;

        // }
        std::vector<std::string> tmp = firstLoc.getIndex();
        for (unsigned int i = 0; i != tmp.size(); ++i)
        {
            loc.setIndex(tmp[i]);
        }
    }

    for (size_t i = 0; firstLoc.getErrmap().size() != i; ++i)
    {
        int flag = 0;
         for (size_t j = 0; loc.getErrmap().size() != j; ++j)
         {
            if (firstLoc.getErrmap()[i] == loc.getErrmap()[j])
            {
                flag = 1;
          
            }
            
         }
        if (flag == 0)
        {
            std::map<std::string, std::string> tmp= firstLoc.getErrorPage();
            loc.setErrorPage(firstLoc.getErrmap()[i], tmp[firstLoc.getErrmap()[i]]);
        }
    }


}


void Conf::organizeServerBlocks()
{
    _server.clear();
    _server.push_back(std::vector<Location>());
    for (size_t i = 0; i < _block.size(); ++i)
    {
        if (!_block[i].getHost().empty() && !_server.back().empty())
        {
            _server.push_back(std::vector<Location>());
            _server.back().push_back(_block[i]);
            // std::cout << "asd"<<_block[i].getAlias()<<std::endl;
        }
        else
        {
            if (_server.back().empty())
            {
                _server.back().push_back(_block[i]);
                continue;
            }
            updateLoc(_block[i]);

_server.back().push_back(_block[i]);

        }
    }
    for (int i = 0; _server[i].size(); ++i)
    {
        if (_server[i].size() == 1 && _server[i][0].getRoot().empty() && _server[i][0].getAlias().empty())
            throw std::runtime_error("empty root or alias");
    }
}

const std::vector<std::vector<Location> >& Conf::getServerBlocks() const {
    return _server;
};

void Conf::printConfig() {
    std::cout << "\n=== Configuration Blocks (" << _server.size() << " server groups) ===\n";

    // 각 서버 그룹을 순회
    for (size_t groupNum = 0; groupNum < _server.size(); ++groupNum) {
        const std::vector<Location>& locations = _server[groupNum];
        
        // 그룹의 첫 번째 Location에서 Host 정보를 가져옴
        std::cout << "\nServer Group " << groupNum + 1;
        if (!locations.empty() && !locations[0].getHost().empty()) {
            std::cout << " (Host: " << locations[0].getHost() << ")";
        }
        std::cout << ":\n";
        std::cout << "========================================\n";

        // 그룹 내의 각 Location 블록을 순회
        for (size_t i = 0; i < locations.size(); ++i) {
            const Location& loc = locations[i];
            
            std::cout << "\n  Location Block " << i + 1 << ":\n";
            std::cout << "  ----------------------------------------\n";
            
            // Basic server configuration
            std::cout << "  Host: " << loc.getHost() << "\n";
            
            std::cout << "  Ports: ";
            const std::vector<unsigned int>& ports = loc.getPort();
            for (size_t j = 0; j < ports.size(); ++j) {
                std::cout << ports[j];
                if (j < ports.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
            
            std::cout << "  Server Names: ";
            const std::vector<std::string>& servers = loc.getServerName();
            for (size_t j = 0; j < servers.size(); ++j) {
                std::cout << servers[j];
                if (j < servers.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
            
            std::cout << "  Client Max Body Size: " << loc.getClientMaxBodySize() << "\n";
            
            // Error pages
            std::cout << "  Error Pages:\n";
            const std::map<std::string, std::string>& errorPages = loc.getErrorPage();
            for (std::map<std::string, std::string>::const_iterator it = errorPages.begin();
                 it != errorPages.end(); ++it) {
                std::cout << "    " << it->first << " -> " << it->second << "\n";
            }
            
            // Location specific configuration
            std::cout << "  Path: " << loc.getPath() << "\n";
            std::cout << "  Root: " << loc.getRoot() << "\n";
            
            std::cout << "  Index Files: ";
            const std::vector<std::string>& indexes = loc.getIndex();
            for (size_t j = 0; j < indexes.size(); ++j) {
                std::cout << indexes[j];
                if (j < indexes.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
            
            std::cout << "  Allowed Methods: ";
            const std::vector<std::string>& methods = loc.getMethods();
            for (size_t j = 0; j < methods.size(); ++j) {
                std::cout << methods[j];
                if (j < methods.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
            
            std::cout << "  Autoindex: " << (loc.getAutoindex() ? "on" : "off") << "\n";
            
            std::cout << "  CGI Paths: ";
            const std::vector<std::string>& cgiPaths = loc.getCgi();
            for (size_t j = 0; j < cgiPaths.size(); ++j) {
                std::cout << cgiPaths[j];
                if (j < cgiPaths.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";

            std::cout << "  Redirect: " << loc.getRedirect() << "\n";

            // std::cout << "\n";
            // std::cout << "  Alias: " << loc.getAlias() << "\n";

            // std::cout << "\n";
        }
    }
    std::cout << "\n========== End of Configuration ==========\n\n";
}