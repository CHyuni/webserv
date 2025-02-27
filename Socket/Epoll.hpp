#ifndef EPOLL_HPP
#define EPOLL_HPP

#include "../ConfigureHeader/Conf.hpp"
#include "../RequestHeader/Request.hpp"
// #include "../MethodHeader/GetHandler.hpp"
#include "../ResponseHeader/Response.hpp"



class Epoll 
{
    private :
        const std::vector<std::vector<Location> >& _config;
        int _epollfd;
        std::vector<int> _socket;
        static const unsigned int MAX_EVENTS = 100; 
        std::map<int, std::string> _result;
        std::map<int, size_t> conLeng;
        std::map<int, size_t> currentLeng;
        std::map<int, Request> request;

        std::map<int, std::string> _pendingResponses;
        std::map<int, std::string> responseMessage;

    public :
        Epoll(const Conf& config);
        Epoll(const Epoll &other);
        Epoll &operator&=(const Epoll &other);
        ~Epoll();
        void run();
        void initSocket();
        void bindSocket(const std::string &host, const unsigned int &port);
        void registerSeverSocket();
        void initClient();
        int isServerSocket(int &fd);
        void handleNewConnection(int &fd);
        void handleRead(int &fd);
        void handleWrite(int &fd);
        void handleClose(int &fd);
        void closeFd();
        void redirect(int &fd);
};

#endif