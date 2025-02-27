#include "../RequestHeader/Request.hpp"

void Request::debug() {

    std::cout << "\n\n\n=== Location ===\n";
    std::cout << _location.getPath() << std::endl;
    std::cout << _location.getHost() << std::endl;
    std::cout << _location.getRedirect() << std::endl;

    // std::cout << "=== Method ===\n";
    // std::cout << this->getMethod();

    std::cout << "\n\n\n=== Path ===\n";
    std::cout << this->getPath();

    std::cout << "\n\n\n=== Normalized Path ===\n";
    std::cout << this->getMappingUrl();

    std::cout << "\n\n\n=== Server Name ===\n";
    std::cout << this->getServerName();

    std::cout << "\n\n\n=== Port ===\n";
    std::cout << this->getServerPort();

    std::cout << "\n\n\n=== Query ===\n";
    std::cout << this->getQuery();

    // std::cout << "\n\n\n=== Version ===\n";
    // std::cout << this->getVersion();

    std::cout << "\n\n\n=== Headers ===\n";
    for (std::map<std::string, std::string>::const_iterator it = this->getHeaders().begin(); it != this->getHeaders().end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    // std::cout << "\n\n\n=== Body ===\n";
    // std::cout << this->getBody();

    // std::cout << "\n\n\n=== Is Parsed ===\n";
    // if (this->_isParsed)
    //     std::cout << "Yes";
    // else
    //     std::cout << "No";

    // std::cout << "\n\n\n=== Is Complete ===\n";
    // std::cout << this->isComplete();

    // std::cout << "\n\n\n=== Content Length ===\n";
    // std::cout << this->getContentLength();

    std::cout << "\n\n\n=== Error Code ===\n";
    std::cout << this->getErrorCode() << "\n";
}