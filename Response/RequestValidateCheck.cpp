#include "../Socket/Epoll.hpp"
#include "../UtilityHeader/Utility.hpp"

std::string Response::getErrorHeader(int error) {
	std::string header;
	switch (error) {
		case 403:
			header = "HTTP/1.1 403 Forbidden";
			break ;
		case 404:
			header = "HTTP/1.1 404 Not Found";
			break ;
		case 405:
			header = "HTTP/1.1 405 Method Not Allowed";
			break;
		case 413:
			header = "HTTP/1.1 413 Payload Too Large";
			break;
		case 500:
			header = "HTTP/1.1 500 Internal Server Error";
			break ;
		case 505:
			header = "HTTP/1.1 505 HTTP Version Not Supported";
			break ;
		default:
			header = "HTTP/1.1 403 Forbidden";
	}
	return header;
}

std::string Response::getErrorPath(int error) {
	std::string path;
	std::map<std::string, std::string>	_error = _loc.getErrorPage();
    std::map<std::string, std::string>::iterator it = _error.find(ToString(error));
    if (it != _error.end()) {
		std::string errorPage = it->second;
		if (access(errorPage.c_str(), F_OK | R_OK) == -1)
			return "ErrorHtml/404.html";
        return it->second;
    }
	return "ErrorHtml/404.html";
}

int Response::getValidate(Request& request) {
	if (!request.getLocation().getCgi().empty())
		return 0;
	std::string path = request.getMappingUrl();
	int error = 0;
	Location loc = request.getLocation();
	int flag = 0;
	for (size_t i = 0; i < loc.getIndex().size(); ++i) {
		std::string temp =  urlDecode(path + loc.getIndex()[i]);
		int errorTemp = checkPermissions(temp.c_str());
		if (errorTemp == 0)
		{
			flag = 1;
			break;
		}
	}
	if ((loc.getIndex().empty() || flag == 0) && request.getMethod() == "GET")
		loc.setIndex("index.html");
	const std::vector<std::string>& index = loc.getIndex();
	std::cout << index.size() <<std::endl;	
	for (size_t i = 0; i < index.size(); ++i) {
		std::string temp =  urlDecode(path + index[i]);
		int errorTemp = checkPermissions(temp.c_str());
		if (!loc.getIndex().empty() && errorTemp == 404 && loc.getAlias().empty())
		{
			errorTemp = 403;
		}
		if (!errorTemp) {
			request.setMappingUrl(temp);
			return 0;
		} else if (!loc.getAutoindex() && !error && errorTemp) error = errorTemp;
	}
	if (error) return error;
	if (isDirectory(path) && !loc.getAutoindex()) return 403;
	return 0;
}

int Response::validateRequest(Request& request) {
	std::string path = urlDecode(request.getMappingUrl());
	std::string version = request.getVersion();
	int error;
	if (request.getPath() == "/upload")
	{
		std::string userId = request.getCookie();
		int length = userId.length();
		if (length < 33)
			return 404;
		int error = checkPermissions("upload/" + userId.substr(0, length - 33));
		return error;
	}
	if (version != "HTTP/1.1" && version != "HTTP/1.0") return 505;
	if (request.getLocation().getCgi().empty())
	{
		error = checkPermissions(path.c_str());
		if (error) 
		{
			return error;
		}
	}
	if (request.getMethod() == "GET" && isDirectory(path.c_str())) {
		error = getValidate(request);
		if (error) return error;
	} else if (request.getMethod() == "POST") {
		error = getValidate(request);
		if (error) return error;
	} else if (request.getMethod() == "DELETE") {

	}
	return 0;
}