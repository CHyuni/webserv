#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "../Inc/WebServer.hpp"
#include "../RequestHeader/Request.hpp"


class Response {
    private:
        std::string _header;
        std::string _body;
        unsigned long long _contentLength;
        std::string getErrorPath(int error);
        std::string getErrorHeader(int error);
        Location _loc;

    public:
        // 생성자
        Response(): _header(""), _body(""), _contentLength(0) {}
        ~Response() {};

        //RequestHandler
        std::string RequestHandler(Request& request);
        std::string textHandler(const Request& request, const std::string& accept);
        std::string imageHandler(const Request& request, const std::string& accept);
        std::string errorHandler(int error);
        std::string postHandler(Request& request);
        std::string autoIndexFile(const Request& request);
        std::string autoIndexHandler(const Request& request);
        std::string removeHandler(Request& request);
        std::string cgiHandler(Request& request);
        std::string executeCgi(const std::vector<std::string>& cgiArgv, Request &request);
        std::string redirectHandler(const std::string &mapPath, const std::string &code);
        void responseNormalized(std::string& response);
        int validateRequest(Request& request);
        int getValidate(Request& request);
        std::string	postUploaded(Request& request, std::string &fileName, const std::string &fileData);
        bool removeAllInDirectory(const std::string& dirPath);


};

#endif