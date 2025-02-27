#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "../Inc/WebServer.hpp"
#include "../ConfigureHeader/Conf.hpp"
#include "../ConfigureHeader/Location.hpp"


class Request {
    private:
        //HTTP 메시지
        std::string _method;       
        std::string _path;      
        std::string _query;   
        std::string _version;      
        std::string _serverName; 
        std::string _port;  
        std::string _mappingUrl;
        std::string _cookie; 
        //post
        std::string _boundary; 

        // 헤더와 바디
        std::map<std::string, std::string> _headers;
        std::string _body;
        size_t _contentLength;

        //서버 conf 파일 설정
        const std::vector<std::vector<Location> > *_conf; 
        int _serverBlockIdx;
        Location _location;

        // 파싱 관련 상태
        bool _isParsed;
        bool _isComplete;
 
        // 에러 관리
        int _errorCode;
            
    public:
        // 생성자
        Request();
        Request(const std::vector<std::vector<Location> > *conf);
        ~Request();

        //request 메인함수
        bool requestHandler(const std::string& rawRequest);

        // 파싱 메소드
        bool parse(const std::string& rawRequest);
        bool parseStartLine(const std::string& line);
        bool parseHeaders(const std::string& headerSection);
        bool parseBody(const std::string& bodyContent);
        bool parseChunkedBody(const std::string& chunkedBody);
            
        // Getter 메소드들
        const std::string& getMethod() const;
        const std::string& getPath() const;
        const std::string& getQuery() const;
        const std::string& getVersion() const;
        const std::string& getBody() const;
        const std::string& getServerName() const;
        const std::string& getServerPort() const;
        const std::string& getMappingUrl() const;
        const std::map<std::string, std::string>& getHeaders() const;
        std::string getHeader(const std::string& key) const;
        int getServerBlockIdx() const;
        const std::vector<std::vector<Location> > *getConfig() const;
        const std::string getAccept() const;
        const Location& getLocation() const;
        const std::string getContentType() const;
        const std::string& getCookie() const;

        // 상태 확인 메소드들
        bool isChunked() const;  // chunked transfer-encoding 확인
        bool isComplete() const;
        bool hasError() const;
        int getErrorCode() const;
        size_t getContentLength() const;  // Content-Length 값 반환

        // Utility 메소드들
        void debug(); //안에 내용 출력
        void setError(int code);  // 에러 설정
		void setMappingUrl(std::string& path);
        void setBody(const std::string& body);
        void setChunkedBody(const std::string& body);
        void reset(const std::vector<std::vector<Location> > *conf);

        //config 파일이랑 연결
        bool getConfigOption();
        int getLocationBlock(int& serverBlockIdx);
        bool isCorretPort(size_t& serverBlockIdx);
        bool isCorreectServername(size_t &idx);
    private:
        // 내부 헬퍼 메소드들
        void parseQueryString(const std::string& url);
        void normalizedPath();  // 경로 정규화 (../와 ./ 처리)
        bool validateRequest();  // 요청의 유효성 검사
};

#endif