#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "../Inc/WebServer.hpp"
class ServerBlock
{
    protected :
        std::string _host;
        std::vector<unsigned int> _port;
        std::vector<std::string> _server;
        unsigned int _size;
		std::string	_root;
		std::vector<std::string>	_methods;
		bool	_autoidx;
		std::vector<std::string>	_index;
		std::map<std::string, std::string>	_error;
		std::vector<std::string>	_errormap;		
        bool						_isServer;
		bool						_isAuto;
        std::string _alias;
        static const unsigned int MAX_BODY_SIZE = 2097152; 
    public :
        ServerBlock();
        ServerBlock(const ServerBlock &other);
        ServerBlock &operator&=(const ServerBlock &other);
        ~ServerBlock();
        static unsigned int strtoul(const std::string &value);
        bool validatePath(const std::string& path);
        bool setHost(const std::string& host);
        bool setPort(unsigned int port);
        bool setServerName(const std::string& server);
        bool setClientMaxBodySize(const std::string &value);
        void setMaxBodySize(const unsigned int &value);
const std::vector<std::string> &getErrmap() const;

        bool setErrorPage(const std::string& num, const std::string& page);
        bool setRoot(const std::string& root);
        bool setIndex(const std::string& index);
        bool setMethods(const std::string& methods);
        bool setAutoindex(const std::string& autoindex);
        bool setAlias(const std::string& alias)
        {
            _alias = alias;
            return true;
        }


        const std::string &getHost() const;
        const std::vector<unsigned int> &getPort() const;
        const std::vector<std::string> &getServerName() const;
        const unsigned int &getClientMaxBodySize() const;
        const std::map<std::string, std::string> &getErrorPage() const;
        const std::string &getRoot() const;
        const std::string &getAlias() const{ return _alias;}
        const std::vector<std::string> &getIndex() const;
        const std::vector<std::string> &getMethods() const;
        const bool &getAutoindex() const;
        const bool &getIsServer() const;
        // const int &getIsServer() const;
        
        void reset();
    };

#endif