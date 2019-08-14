#ifndef SEARCH_WORK_H
#define SEARCH_WORK_H

#include "../common/log.h"
#include "../common/doc_util.h"
#include "../database/redis.h"
#include <unordered_map>
#include <cppjieba/include/cppjieba/Jieba.hpp>

BEGIN_NAMESPACE(search_server, service);

class SearchWork {
public:
    SearchWork();

    bool Init();

    std::string Insert(std::string param);

    std::string Search(std::string param);

private:
    void CutWord(std::string& input, std::vector<std::string>* output);

    std::string MakeDesc(const std::string& content, std::string key);
    
private:
    database::Redis                                             mRedis;
    strategy::InsertWork                                        mInsert;
    cppjieba::Jieba                                             mJieba;
    std::unordered_map<std::string, std::vector<util::Weight>>  mIndex;
};

END_NAMESPACE;
#endif
