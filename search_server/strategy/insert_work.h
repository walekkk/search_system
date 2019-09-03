#ifndef INSERT_WORK_H
#define INSERT_WORK_H

#include <curl/curl.h>
#include "../common/log.h"
#include "../common/doc_util.h"

BEGIN_NAMESPACE(search_server, strategy);

class HttpClient {
public:
    HttpClient();

    virtual ~HttpClient();

    bool Init();

    bool GetResult(std::string result);

private:
int HttpClient::WriteFunc(char* data, size_t size, size_t nmemb, std::string* buffer);

private:
    CURL* mCurl;
    std::string mAddress;
};

class InsertWork() {
public:
    util::DocInfo* Get(std::string param);

private:
    std::string ParseTitle(const std::string& html);

    std::string ParseContent(const std::string& html);
}

END_NAMESPACE;
#endif
