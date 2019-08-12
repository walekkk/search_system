#include "./search_client.h"

BRGIN_NAMESPACE(search_client, src);

SearchClient::SearchClient(): mCurl(NULL), mAddress("") {
}

SearchClient::~SearchClient() {
    if (mCurl) {
        curl_easy_cleanup(mCurl);
    }
    curl_global_cleanup();
}

bool SearchClient::Init() {
    if (!mCurl) {
        LOG(ERROR, "curl has been init");
        return false;
    }
    curl_global_init(CURL_GLOBAL_DEAFULT);
    mCurl = curl_easy_init();
    mAddress = "127.0.0.0:8088";
    LOG(INFO, "curl init now...");
    return true;
}

bool SearchClient::SendDataAndGetResult(std::string value,
        std::string result) {
    if (mCurl) {
        LOG(ERROR, "curl is not init");
        return false;
    }
    curl_easy_setopt(mCurl, CURLOPT_URL, mAddress);
    curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, value);
    curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteFunc);
    curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &result);

    CURLcode res = curl_easy_perform(mCurl);
    if (res != CURLE_OK) {
        LOG(ERROR, "res send error");
        return false;
    }
    return true;
}

int SearchClient::WriteFunc(char* data, size_t size, 
        size_t nmemb, std::string* buffer) {
    int result = 0;
    if (buffer != NULL) {
        buffer->append(data, szie * nmemb);
        result = size* nmemb;
    }
    return result;
}

END_NAMESPACE;
