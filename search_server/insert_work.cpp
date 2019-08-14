#include "../common/log.h"
#include "./insert_work.h"

BEGIN_NAMESPACE(search_server, strategy);

HttpClient::HttpClient(std::string address):
     mCurl(NULL), mAddress(address){
}

HttpClient::~HttpClient() {
    if (mCurl) {
        curl_easy_cleanup(mCurl);
    }
    curl_global_cleanup();
}

bool HttpClient::Init() {
    if (mCurl != NULL) {
        LOG(ERROR, "curl has been init");
        return false;
    }
    curl_global_init(CURL_GLOBAL_DEFAULT);
    mCurl = curl_easy_init();
    LOG(INFO, "curl init now...");
    return true;
}

bool HttpClient::GetResult(std::string result) {
    if (mCurl == NULL) {
        LOG(ERROR, "curl is not init");
        return false;
    }
    curl_easy_setopt(mCurl, CURLOPT_URL, mAddress);
    curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteFunc);
    curl_easy_setopt(mCurl, CURLOPT_WEITEDATA, &result);

    CURLcode res = curl_easy_perform(mCurl);
    if (res != CURLE_OK) {
        LOG(ERROR, "res send error");
        return false;
    }
    return true;
}

int HttpClient::WriteFunc(char* data, size_t size, size_t nmemb, std::string* buffer) {
    int result = 0;
    if (buffer != NULL) {
        buffer->append(data, size* nmemb);
        result = size * nmemb;
    }
    return result;
}

util::DocInfo* InsertWork::Get(std::string param) {
    DocInfo* pDocInfo;
    pDocInfo->mUrl = param;
    HttpClient client(param);
    client.Init();
    std::string& result;
    if (!client.GetResult(result)) {
        LOG(ERROR "dont have this url");
        return NULL;
    }
     
    pDocInfo->mContent =  ParseContent(result);
    pDocInfo->mTitle = ParseTitle(result);
    if (mContent == "" || mTitle == "") {
        LOG(ERROR, "create html error");
        return NULL;
    }
    return pDocInfo;
}

std::string InsertWork::ParseTitle(const std::string& html) {
   size_t begin = html.find("<title>");
   if (begin == std::string::npos) {
       return "";
   }

   size_t end = html.find("</title>");
   if (end == std::string::npos) {
       return "";
   }

   begin += std::string("<title>").size();
   if (begin > end) {
       return "";
   }

   std::string result("");
   result = html.substr(begin, end - begin);
   return result;
}

std::string InsertWork::ParseContent(const std::string& html) {
    bool isContent = true;
    std::string result;
    for (auto c : html) {
        if (isContent) {
            if (c == '<') {
                isContent = false;
                continue;
            } else {
                if (c == '\n') {
                    c= ' ';
                }
                result += c;
            }
        } else {
            if (c == '>') {
                isContent = true;
            }
        }
    }
    return result;
}

END_NAMESPACE;
