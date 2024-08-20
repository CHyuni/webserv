#include "../inc/Location.hpp"

void Location::setPath(const std::string& path) {
	std::string temp;
	size_t i = 0;

	while (i < path.length() && isspace(path[i]) && path[i] != '{')
		i++;
	if (path[i] == '\0' || path[i] == '{')
		return ; // < throw expected
	while (i < path.length() && !isspace(path[i]) && path[i] != '{')
		temp += path[i++];
	_path = temp;
}
