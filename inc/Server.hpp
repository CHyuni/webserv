#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"

class Server
{
	protected:
		std::vector<std::string>	_serverName;
		u_int16_t					_port;
		std::string					_host;
		long						_maxBodySize;
		std::string					_root;
		std::vector<std::string>	_methods;
		bool						_autoidx;
		std::vector<std::string>	_index;
		std::vector<std::string>	_errorPage;

	public:
		Server();
		virtual ~Server();
		Server(const Server& rhs);
		Server& operator=(const Server& rhs);

		void	setName(const std::string& n);
		void	setPort(const std::string& p);
		void	setHost(const std::string& h);
		void	setSize(const std::string& s);
		void	setRoot(const std::string& r);
		void	setError(const std::string& e);
		void	setIndex(const std::string& i);
		void	setAutoidx(const std::string& ai);
		void	setMethods(const std::string& m);
};

#endif