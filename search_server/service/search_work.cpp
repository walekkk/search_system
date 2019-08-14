#include "./search_work.h"
#include <boost/algorithm/string.hpp>
#include<jsoncpp/json/json.h>

BEGIN_NAMESPACE(search_server, service);

const char* const DICT_PATH = "jieba_dict/jieba.dict.utf8";
const char* const HMM_PATH = "jieba_dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "jieba_dict/user.dict.utf8";
const char* const IDF_PATH = "jieba_dict/idf.utf8";
const char* const STOP_WORD_PATH = "jieba_dict/stop_words.utf8";

SearchWork() {
    mJieba(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH);
}

void SearchWork::Init() {
    if (!mRedis.Init()) {
        return false;
    }
    return true;
}

std::string SearchWork::Insert(std::string param) {
    util::DocInfo* docInfo(NULL);
    if ((docInfo = mInsert.Get(param)) == NULL) {
        return "";
    }   
    if (!mRedis.Push(docInfo)) {
        return false;
    }
    std::vector<std::string> titleTokens;
    CutWord(docInfo->mTitle, &titleTokens);
    std::vector<std::string> contentTokens;
    CutWord(docInfo->mContent, &contentTokens);

    struct WordCount {
        int mTitleCount;
        int mContentCount;
    };
    std::unordered_map<std::string, WordCount> wordCount;
    for (std::string& word : titleTokens) {
        boost::to_lower(word);
        ++(wordCount[word].mTitleCount);
    }
    for (std::string& word : contentTokens) {
        boost::to_lower(word);
        ++(wordCount[word].mContentCount);
    }
    
    for (auto& iter : wordCount) {
        util::Weight weight;
        weight.mUrl = docInfo->mUrl;
        weight.mWeight = 10 * ((iter.second).mTitleCount) + (iter.second).mContentCount;
        weight.mKey = iter.first;
        for (int index = 0; index < mIndex[iter.first].size(); index ++) {
            if (weight.mWeight < mIndex[iter.first][index].mWright) {
                continue;
            }
            mIndex[iter.first].push_back(weight);
            for (int cnt = mIndex[iter.first].size()-2; cnt <= index; cnt--) {
                mIndex[iter.first][cnt] = mIndex[iter.first][cnt+1];
            }
            mIndex[iter.first][index] = weight;
            break;
        }
        if (mIndex[iter.first].size() == 0 || 
                mIndex[iter.first][mIndex[iter.first].size()-1].mWeight > weight.mWeight) {
            mIndex[iter.first].push_back(weight);
        }
    }
    return "insert success!";
}

bool SearchWork::CutWord(std::string& input, std::vector<std::string>* output) {
    mJieba.CutForSearch(input, *output);
}

std::string Search(std::string param) {
    std::vector<util::DocInfo> resultVector;
    if (mIndex[param].size() == 0) {
        return "";
    }
    for (int index = 0; index < mIndex[param].size(); index++) {
        resultVector.push_back(mRedis.Get(mIndex[param][index].mUrl));
    }
    //到这里，rsultVector里存放按照顺序排列好的docInfo

    Json::Value Results;
    for (auto& docInfo : resultVector) {
        Json::Value result;
        result["desc"] = MakeDesc(docInfo.mContent, param);
        result["url"] = docInfo.mUrl;
        result["title"] = docInfo.mTitle;
        Results.append(result);
    }
    Json::StyleWriter writer;
    return writer.write(Results);
}

std::string SearchWork::MakeDesc(const std::string& content, std::string key) {
    size_t pos = content,find(key);
    if (pos == std::string::npos) {
        if (content.size() <100) {
            return content;
        } else {
            return content.substr(0, 100) + "...";
        }
    }

    size_t begin = pos < 50 ? 0 : pos - 50;
    if (pos + 50 >= content.size()) {
        return content.substr(begin);
    } else {
        return content.substr(begin, 100) + "...";
    }
}
END_NAMESPACE;
