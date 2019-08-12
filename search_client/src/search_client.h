#ifndef SEARCH_CLIENT_H
#define SEARCH_CLIENT_H

#include <curl/curl.h>
#include "../common/log.h"

BEGIN_NAMESPACE(search_client, src);

class SearchClient {
public:
    SearchClient();

    virtual ~SearchClient();

    bool Init();

    bool SendDataAndGetResult(std::string value, std::string result);

private:
    int WriteFunc(char* data, size_t size, size_t nmemb, std::string* buffer);

private:
    CURL*           mCurl;
    std::syting     mAddress;
};

END_NAMESPACE;
#endif
