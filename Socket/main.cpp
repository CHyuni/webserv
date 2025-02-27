#include "Epoll.hpp"

int main(int args, char **argv)
{
    Epoll* epoll = NULL;

    try
    {
        Conf conf(Conf::checkInput(args, argv[1]));
        conf.parseConf();
        //Request req;
        //req.debug();
        epoll = new Epoll(conf);
        epoll->run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        if (epoll)
        {
            epoll->closeFd();
            delete epoll;
        }
    }
    return 0;
}