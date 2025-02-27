#ifndef CONF_HPP
#define CONF_HPP

#include "Location.hpp"
#include "../Inc/WebServer.hpp"

enum ErrorType
{
    TOO_MANY_ARGS,
    FILE_NOT_FOUND,
    INVALID_FORMAT,
};

enum ConfigType {
    HOST,
    LISTEN,
    SERVER_NAME,
    CLIENT_MAX_BODY_SIZE,
    ERROR_PAGE,
    LOCATION,
    ROOT,
    INDEX,
    METHODS,
    AUTOINDEX,
    PATH,
    CGI_PATH,
    REDIRECT,
    ALIAS
};

class Conf
{
    private :
        std::ifstream _file;
        std::string _line;
        std::vector<Location> _block;
        Location* _back;
        std::map<std::string, ConfigType> _confMap;
        std::vector<std::vector<Location> > _server;
    public :
        class InputErrException;
        static char* checkInput(const int &args, char* &argv);
        Conf(const std::string &file);
        Conf(const Conf &other);
        Conf &operator&=(const Conf &other);
        ~Conf();
        void    parseConf();
        void    makeLine();
        bool    checkBrace();
        bool    switchCase(const std::string &key, const std::string &val);
        bool    parseCommon(const std::string &key, const std::string &value, std::string::const_iterator &i);
        bool    parseErrorpage(const std::string &value, std::string::const_iterator &i);
        bool    parseLoc(std::string &key, std::string::const_iterator &start, std::string::const_iterator &i);
        bool    parseBlock(std::string::const_iterator &i);
        bool    parseLine(std::string::const_iterator &i);
        bool    putBlock(const std::string &key, const std::string &value, std::string::const_iterator &i);
        bool    parseLocation(std::string::const_iterator &i);
        bool    putLocation(const std::string &key, const std::string &value, Location& currentLocation, std::string::const_iterator &i);    
        bool    putMethods(const std::string &value, Location& currentLocation, std::string::const_iterator &i);    
        bool    allowConf(const std::string &conf);
        bool    isFirstLine(std::string::const_iterator &i);
        bool    isSpace(std::string::const_iterator &i);
        void    updateLoc(Location &loc);
        void    organizeServerBlocks();
        void    printConfig();
        const std::vector<std::vector<Location> >& getServerBlocks() const;


};

#endif