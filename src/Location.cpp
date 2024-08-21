#include "../inc/Location.hpp"

void Location::setPath(const std::string& path) {
	std::string temp;
	size_t i = 0;

	while (i < path.length() && isspace(path[i]))
		i++;
	if (path[i] == '\0' || path[i] == '{')
		throw std::invalid_argument("invalid"); // < throw expected
	while (i < path.length() && !isspace(path[i]))
		temp += path[i++];
	if (!temp.empty()) {
		_path = temp;
	} else
		throw std::invalid_argument("invalid");
}

std::string Location::getPath() {
	return _path;
}