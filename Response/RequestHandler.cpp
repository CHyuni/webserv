#include "../Socket/Epoll.hpp"
#include "../UtilityHeader/Utility.hpp"

std::string Response::errorHandler(int error) {
	std::string path = getErrorPath(error);
	std::string header = getErrorHeader(error);
	std::ifstream file(path.c_str());
	std::string html;
	if (!file.is_open()) {
        header = getErrorHeader(500);
        path = getErrorPath(500);
        html =
	        "<!DOCTYPE html>\n"
	        "<html>\n"
	        "<head>\n"
	        "    <meta charset=\"UTF-8\">\n"
	        "    <title>500</title>\n"
	        "</head>\n"
	        "<body>\n"
	        "    500\n"
	        "</body>\n"
	        "</html>\n";
    } else {
        std::string line;
        while (std::getline(file, line)) {
            html += line + "\n";
        }
    }
	std::ostringstream ss;
	ss << html.size();

	std::string response = header + "\r\n";
	response += "Content-Type: text/html; charset=UTF-8\r\n";
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "Date: " + getGMTDate() + "\r\n\r\n";
	response += html;

	return response;
}

std::string Response::autoIndexFile(const Request& request) {
    size_t pos = request.getMappingUrl().rfind("/");
    std::string filename = request.getMappingUrl().substr(pos + 1);
    std::ifstream file(request.getMappingUrl().c_str(), std::ios::binary);
    if (!file.is_open()) return errorHandler(500);
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);
    std::string response;

    response += "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n";
    response += "Content-Disposition: attachment; filename=\"" + filename + "\"\r\n";
    response += "Content-Length: " + ToString(size) + "\r\n";
    response += getGMTDate() + "\r\n";
    response += "Connection: keep-alive\r\n\r\n";

    response.reserve(response.size() + size);

    // 버퍼에 파일 내용 읽기
	const size_t buffer_size = 8192;
    std::vector<char> buffer(buffer_size);
    size_t bytes_read = 0;
	size_t remaining = size;
	while (remaining) {
		remaining -= bytes_read;
		size_t read_size = std::min(remaining, buffer_size);
		file.read(&buffer[0], read_size);
		bytes_read = file.gcount();
		// 이미지 데이터 추가
		response.append(&buffer[0], bytes_read);
	}

    return response;
}

std::string Response::autoIndexHandler(const Request& request) {
    std::string mapPath = request.getMappingUrl();
    DIR* dir = NULL;
    if (isDirectory(mapPath)) {
        dir = opendir(mapPath.c_str());
        if (!dir) {
            return errorHandler(500);
        }
    } else if (!isDirectory(mapPath) && request.getLocation().getAutoindex()) return autoIndexFile(request);
    std::string result;
    std::string path = request.getPath();
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue ;

        std::string filename;
        if (path[path.size() - 1] != '/') {
            filename += '/';
        }
        filename += entry->d_name;
        std::string _path = path + filename;
        std::string _mapPath = mapPath + filename;

        filename = entry->d_name;
        struct stat file_info;
        if (!stat(_mapPath.c_str(), &file_info)) {
            char time_str[100];
            struct tm* tm_info = localtime(&file_info.st_mtime);
            strftime(time_str, sizeof(time_str), "%Y년 %m월 %d일 %H시 %M분 %S초", tm_info);
            result += "<a href=\"" + _path;
            if (isDirectory(_path))
                result += "/";
            result += "\">" + filename + "</a> " + time_str + "\n";
        } else {
            return errorHandler(500);
        }
    }
    closedir(dir);
    return result;
}
std::string Response::redirectHandler(const std::string &mapPath, const std::string &code)
{
    std::string ss = code + " Moved Permanently";
    std:: string response = "HTTP/1.1 " + code + " Moved Permanently\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + ToString(ss.size()) + "\r\n";
    response += "Location: " + mapPath + "\r\n";
	response += "Date: " + getGMTDate() + "\r\n";
    response += "Connection: keep-alive\r\n\r\n";
    response += ss;
    return response;
}


std::string Response::textHandler(const Request& request, const std::string& accept) {
	std::string mapPath = request.getMappingUrl();
    std::string html;

    if (request.getMethod() != "GET" && !strncmp(request.getPath().c_str(), "/upload", 7))
	{
		std::string body = "success\r\n";
		std::string response = "HTTP/1.1 200 OK\r\n";
	    response += "Content-Type: text/plain; charset=UTF-8\r\n";
	    response += "Content-Length: 7\r\n";
		response += "Date: " + getGMTDate() + "\r\n\r\n";
		response += body;
		return response;
	}
	if (isDirectory(mapPath)) {
        html = autoIndexHandler(request);
    } else if (checkDownload(mapPath) && request.getLocation().getAutoindex()) {
        return autoIndexHandler(request);
    } else if (!checkDownload(mapPath) && request.getLocation().getAutoindex()) {
        std::string _accept = reGetAccept(mapPath.substr(mapPath.find(".") + 1));
        return imageHandler(request, _accept);
    } else {
        std::ifstream file(request.getMappingUrl().c_str());
        if (!file.is_open()) return errorHandler(500);
            std::string line;
        while (std::getline(file, line)) {
            html += line + "\n";
        }
    }
    
    std::ostringstream ss;
    ss << html.size();
    
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + accept + "; charset=UTF-8\r\n";
    response += "Content-Length: " + ss.str() + "\r\n";
	response += "Date: " + getGMTDate() + "\r\n\r\n";
    response += html;

    return response;
}

std::string Response::imageHandler(const Request& request, const std::string& accept) {
	std::ifstream file(urlDecode(request.getMappingUrl()).c_str(), std::ios::binary);
    if (!file.is_open()) return errorHandler(500);

    // 파일 크기 확인
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

	std::ostringstream oss;
    oss << size;

    // HTTP 응답 생성
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + accept + "\r\n";
    response += "Content-Length: " + oss.str() + "\r\n";
	response += "Date: " + getGMTDate() + "\r\n\r\n";

	response.reserve(response.size() + size);

    // 버퍼에 파일 내용 읽기
	const size_t buffer_size = 8192;
    std::vector<char> buffer(buffer_size);
    size_t bytes_read = 0;
	size_t remaining = size;
	while (remaining) {
		remaining -= bytes_read;
		size_t read_size = std::min(remaining, buffer_size);
		file.read(&buffer[0], read_size);
		bytes_read = file.gcount();
		// 이미지 데이터 추가
		response.append(&buffer[0], bytes_read);
	}

    return response;
}

std::string	Response::postUploaded(Request& request, std::string &fileName, const std::string &fileData) {
	std::string userId, cookie;
	size_t cookieLength;

	cookie = request.getCookie();
	cookieLength = cookie.length();
	if (fileName.empty() || fileData.empty()) {
        std::string res;
        res = "HTTP/1.1 204 No Content\r\n";
        res += "Date: " + getGMTDate() + "\r\n";
        res += "Content-Length: 0\r\n\r\n";
        return res;
    }
    if (cookieLength < 34)
    {
        return errorHandler(500); 
    }
    userId = cookie.substr(0, cookieLength - 33);
	if (access("upload", F_OK | W_OK) == -1) {  
		std::cerr << "Directory 'upload' does not exist or is not writable" << std::endl;
		return errorHandler(500);
	}
	std::string dirPath = request.getMappingUrl() + "/" + userId;

	struct stat info;
	if (stat(dirPath.c_str(), &info) == -1)
	{
		//디렉토리가 없음
		if (mkdir(dirPath.c_str(), 0777) == -1)
		{
			std::cerr << "디렉토리 생성 실패: " << strerror(errno) << std::endl;
			return errorHandler(500);
		} 
		std::cout << "디렉토리 생성 성공: " << dirPath << std::endl;
	} else if (info.st_mode & S_IFDIR) {
		if (!removeAllInDirectory(dirPath))
			return errorHandler(500);
		std::cout << "디렉토리가 이미 존재합니다: " << dirPath << std::endl;
	} else {
		std::cerr << "디렉토리 생성 실패: 해당 경로에 디렉토리가 아님" << std::endl;
		return errorHandler(500);
	}

	std::string filepath = "upload/" + userId + "/" + fileName;

    std::ofstream outFile(filepath.c_str(), std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return errorHandler(500);
    }
    outFile.write(fileData.c_str(), fileData.length());
    outFile.close();
    return textHandler(request, request.getAccept());
}

std::string Response::postHandler(Request& request) {

    const std::string& body = request.getBody();
    std::string boundary;
    
    size_t pos = 0;
    std::string fileName, fileData, userId;
    std::string contentType;

    std::cout << "postHandler function called\n";

    pos = body.find("\r\n", pos);
    if (pos == std::string::npos && !request.isChunked())
    {
        std::string res;
        res = "HTTP/1.1 204 No Content\r\n";
        res += "Date: " + getGMTDate() + "\r\n";
        res += "Content-Length: 0\r\n\r\n";
        return res;
    }
	boundary = body.substr(0, pos - 2);
	// std::cout <<boundary <<std::endl;
    pos = pos + 2; 
    	// std::cout<< "@@@@@@3"<<std::endl;
    while (pos < body.length()) {
        size_t lineEnd = body.find("\r\n", pos);
        if (lineEnd == std::string::npos) break; //캐리지 못찾을시
        std::string line = body.substr(pos, lineEnd - pos); // 캐리지제외라인
        
        if (line.find("Content-Disposition:") != std::string::npos) {
            size_t fnPos = line.find("filename=\"");
            if (fnPos != std::string::npos) {
                fnPos += 10; 
                size_t fnEnd = line.find("\"", fnPos);
                if (fnEnd != std::string::npos) {
                    fileName = line.substr(fnPos, fnEnd - fnPos);
                }
            }
        }
        else if (line.find("Content-Type:") != std::string::npos) {
            contentType = line.substr(line.find(":") + 2);
        }
        else if (line.empty()) //r/n 하나 이후 실제이진파일 시작
		{
            pos = lineEnd + 2;  // 빈줄 \r\n 건너뛰기
            // 다음 boundary까지의 데이터가 파일 내용
            size_t nextBoundary = body.find(boundary, pos);
            if (nextBoundary != std::string::npos) {
                fileData = body.substr(pos, nextBoundary - pos - 2);  //마지막  \r\n 제거
                break;  // 파일 데이터를 찾았으므로 반복 종료
            }
        }
        
        pos = lineEnd + 2;  // 다음 줄로 이동
    }

    return postUploaded(request, fileName, fileData); 
}

std::string Response::removeHandler(Request& request)
{
    std::string filepath = request.getMappingUrl();
    
    remove(request.getMappingUrl().c_str());
    return textHandler(request, request.getAccept());
}
std::vector<std::string> createEnvStrings(const Request& request) {
    std::vector<std::string> env_strings;
    
    // 문자열들을 벡터에 저장
    env_strings.push_back("REQUEST_METHOD=" + request.getMethod());
    env_strings.push_back("CONTENT_LENGTH=" + ToString(request.getContentLength()));
    std::string contentType = request.getHeader("Content-Type");
    if (!contentType.empty()) {
        env_strings.push_back("CONTENT_TYPE=" + contentType);
    }
    env_strings.push_back("PATH_INFO=/home/ksuh/goinfre/42webserv/Code/YoupiBanane/youpi.bla");
    env_strings.push_back("QUERY_STRING=" + request.getQuery());
    env_strings.push_back("REDIRECT_STATUS=200");
    std::string secretType = request.getHeader("X-Secret-Header-For-Test");
    if (!secretType.empty()) {
        env_strings.push_back("HTTP_X_SECRET_HEADER_FOR_TEST=" + secretType);
    }
    env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");
    std::string cookie = request.getCookie();
    if (!cookie.empty()) {
        env_strings.push_back("HTTP_COOKIE=" + cookie);
    }
    
    return env_strings;
}

void writePost(Request &request) {
    if (request.getMethod() == "POST") {
        
        char tmpfile[] = "tmp_cgi";
        int tmpfd = open(tmpfile, O_WRONLY | O_CREAT, 0644);
        if (tmpfd != -1) {
           if (-1 == write(tmpfd, request.getBody().c_str(), request.getBody().length()))
            close(tmpfd);
        }
        tmpfd = open(tmpfile, O_RDONLY);
        if (tmpfd != -1) {
            dup2(tmpfd, 0);
            close(tmpfd);
        }
        unlink(tmpfile);
    }
}
std::string Response::executeCgi(const std::vector<std::string>& cgiArgv, Request &request)
{
    int fd[2];
    pid_t pid;
    if (pipe(fd) == -1)
        return errorHandler(500);
    
    pid = fork();
    if (pid == -1) {
        close(fd[0]);
        close(fd[1]);
        return errorHandler(500);
    }
    
    if (pid == 0) { 
        alarm(100);
        close(fd[0]);
    if (request.getMethod() == "GET")
    {
    int devnull = open("/dev/null", O_RDONLY);
    if (devnull == -1) {
        perror("open /dev/null failed");
        exit(1);
    }
    if (dup2(devnull, 0) == -1) {
        perror("dup2 stdin failed");    
        exit(1);                        
    }
        close(devnull);
    }
        writePost(request);
        std::vector<const char*> argv;
        for (size_t i = 0; i < cgiArgv.size(); ++i) {
            argv.push_back(cgiArgv[i].c_str());
        }
        argv.push_back(NULL);

        std::vector<std::string> env_strings = createEnvStrings(request);
        std::vector<char*> env;
        for (size_t i = 0; i < env_strings.size(); ++i) {
            env.push_back(const_cast<char*>(env_strings[i].c_str()));
        }
        env.push_back(NULL);

        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 failed");
            exit(1);
        }
        close(fd[1]);
        execve(argv[0], (char *const *)&argv[0], env.data());
        perror("execve failed");
        exit(1);
    }

    close(fd[1]);

    char buffer[4096];
    std::string response;
    ssize_t bytes;
    memset(buffer, sizeof(buffer), sizeof(char));
    while ((bytes = read(fd[0], buffer, sizeof(buffer))) > 0) {
        response.append(buffer, bytes);
    }

    close(fd[0]);

    int status;
    waitpid(pid, &status, 0);

    if (response.empty()) {
        return errorHandler(500);
    }
    std::string head = response.substr(0, response.find("\r\n\r\n") + 4);
    size_t pos = response.find("\r\n\r\n");
    response = response.substr(pos + 4);
    if (head.find("200 OK\r\n") != std::string::npos && head.find("HTTP/1.1") == std::string::npos)
    {
        size_t pos = head.find("\r\n");
        std::string origin = head.substr(0, pos);
        head.replace(0, origin.length(), "HTTP/1.1 200 OK");
    }

    if (head.find("Content-Length") == std::string::npos) {
        size_t postemp = head.find("\r\n\r\n");
        head.insert(postemp + 2, createContentLength(response));
    }
    return  head + response;
}

std::string Response::cgiHandler(Request& request)
{
    std::vector<std::string> cgiArgv;
    cgiArgv.push_back(request.getLocation().getCgi()[0]);
    cgiArgv.push_back(request.getMappingUrl());

    std::string method = request.getMethod();
    std::string response = executeCgi(cgiArgv, request);

    std::cout << "\n==============cgi response==============\n";
    // std::cout << response;   
    std::cout << "\n=======================================\n\n";

    return response;
}

std::string Response::RequestHandler(Request& request) {
	if (request.getPath().find(".ico") != std::string::npos) {
		std::string fa = "/home/myeochoi/42webserv/Code/html/image/favi.ico";
		request.setMappingUrl(fa);
		return imageHandler(request, "image/x-icon");
	}
    _loc = request.getLocation();
    if (!request.getLocation().getRedirect().empty())
    {
        return redirectHandler(request.getLocation().getRedirect() , "302");
    }
    // std::cout <<"gepath:"<<request.getPath()<<"|"<< isDirectory(request.getMappingUrl() )<<"|" << request.getMappingUrl()<<std::endl;
    if ((request.getMethod() == "GET" && request.getPath() == "/upload") || (request.getPath() != "/upload" && isDirectory(request.getMappingUrl()) && request.getPath()[request.getPath().size() - 1] != '/')) {
        return redirectHandler("http://" + request.getHeader("Host") + request.getPath() + '/', "301");
    }
	int error = validateRequest(request);
    std::cout <<"error: " << error<<std::endl;
	if (error) return errorHandler(error);
	if (!request.getLocation().getCgi().empty())
	{
        std::string tt = cgiHandler(request);
		return(tt);
	}
	if (request.getMethod() == "GET") {
		std::string accept = request.getAccept();

		if (accept.find("text") != std::string::npos) {
			return textHandler(request, accept);
		} else if (accept.find("image") != std::string::npos) {
			return imageHandler(request, accept);
		} else {
			return textHandler(request, accept);
		}
	} else if (request.getMethod() == "POST") {
		return postHandler(request);
	} else if (request.getMethod() == "DELETE") {
		return removeHandler(request);
	}
	return 0;
}

bool Response::removeAllInDirectory(const std::string& dirPath) {
    DIR *dir = opendir(dirPath.c_str());
    if (dir == NULL) {
        std::cerr << "디렉토리 열기 실패: " << strerror(errno) << std::endl;
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string entry_name = entry->d_name;

        // "."과 ".."은 무시
        if (entry_name == "." || entry_name == "..") {
            continue;
        }

        std::string full_path = dirPath + "/" + entry_name;
        struct stat stat_buf;

        if (stat(full_path.c_str(), &stat_buf) == 0) {
            // 파일이면 삭제
            if (S_ISREG(stat_buf.st_mode)) {
                if (unlink(full_path.c_str()) != 0) {
                    std::cerr << "파일 삭제 실패: " << strerror(errno) << std::endl;
                    closedir(dir);
                    return false;
                }
                std::cout << "파일 삭제: " << full_path << std::endl;
            }
            // 디렉토리이면 재귀적으로 삭제
            else if (S_ISDIR(stat_buf.st_mode)) {
                if (!removeAllInDirectory(full_path)) {
                    closedir(dir);
                    return false;
                }
                if (rmdir(full_path.c_str()) != 0) {
                    std::cerr << "디렉토리 삭제 실패: " << strerror(errno) << std::endl;
                    closedir(dir);
                    return false;
                }
                std::cout << "디렉토리 삭제: " << full_path << std::endl;
            }
        } else {
            std::cerr << "경로 확인 실패: " << strerror(errno) << std::endl;
            closedir(dir);
            return false;
        }
    }

    closedir(dir);
    return true;
}