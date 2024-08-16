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
		std::map<std::string, std::string>	_errorPage;

	public:
		Server();
		virtual ~Server();
		Server(const Server& rhs);
		Server& operator=(const Server& rhs);

		void	setName(const std::string& str);
		void	setPort(const std::string& str);
		void	setHost(const std::string& str);
		void	setSize(const std::string& str);
		void	setRoot(const std::string& str);
		void	setError(const std::string& str);
		void	setIndex(const std::string& str);
		void	setAutoidx(const std::string& str);
		void	setMethods(const std::string& str);
};

#endif