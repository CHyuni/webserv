#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"

class Location;

class Server
{
	protected:
		std::vector<std::string>	_serverName;
		std::vector<u_int16_t>		_port;
		std::string					_host;
		long						_maxBodySize;
		std::string					_root;
		std::vector<std::string>	_methods;
		bool						_autoidx;
		std::vector<std::string>	_index;
		std::map<std::string, std::vector<std::string> >	_errorPage;
	
	private:
		std::map<std::string, Location> _locationMap;

	public:
		Server();
		virtual ~Server();
		Server(const Server& rhs);
		Server& operator=(const Server& rhs);

		std::string	getName(size_t i);
		u_int16_t	getPort(size_t i);
		std::string	getHost();
		long		getSize();
		std::string	getRoot();
		std::string	getMethods(size_t i);
		bool		getAutoidx();
		std::string	getIndex(size_t i);
		std::string	getError(const std::string& rhs, size_t i);
		Location	getLocation(const std::string& str);

		void	setName(const std::string& str);
		void	setPort(const std::string& str);
		void	setHost(const std::string& str);
		void	setSize(const std::string& str);
		void	setRoot(const std::string& str);
		void	setError(const std::string& str);
		void	setIndex(const std::string& str);
		void	setAutoidx(const std::string& str);
		void	setMethods(const std::string& str);
		void	setLocation(const std::string& str, const Location& loca);
};

#endif