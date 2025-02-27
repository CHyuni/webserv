#ifndef LOCATION_HPP
#define LOCATION_HPP


#include "ServerBlock.hpp"
#include "../Inc/WebServer.hpp"

class Location : public ServerBlock 
{
    private :
        std::string _path;
        std::vector<std::string> _cgi;
        std::string _redirect;

    public :
        Location();
        Location(const Location &other);
        Location &operator&=(const Location &other);
        ~Location();
        bool setPath(const std::string& path);
        bool setCgi(const std::string& cgi);
        bool setRedirect(const std::string& cgi);
		void setServer(void);
        
        const std::string &getPath() const;
        const std::vector<std::string> &getCgi() const;
        const std::string &getRedirect() const;

        void reset();
};

#endif