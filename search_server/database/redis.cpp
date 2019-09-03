#include <iostream>
#include "../common/common.h"
#include "./redis.h"

BEGIN_NAMESPACE(search_server, database);

Redis::Redis(): mRedis(NULL), mAddress("") {}

Redis::~Redis() {
    if (mRedis) {
        redisFree(mRedis);
        LOG(INFO, "redis free success");
    }
}

bool Redis::Init() {
    mPort = 8081;
    *mIp = "127.0.0.0";
    mRedis = redisConnect(mIp, mPort);
    if (!mRedis) {
        LOG(ERROR, "redis init error");
        return false;
    }
    return true;
}

bool Redis::Push(std::string& key, std::string& value) {
    if (!mRedis) {
        LOG(ERROR, "redis not init now");
        return false;
    } 
    redisReply* reply = (redisReply*)redisCommand(mRedis, "SET %s %s",
            key.c_str(), value.c_str());
    if (!reply) {
        LOG(ERROR, "redis insert error");
        return false;
    } else if (reply->str != "OK") {
        LOG(ERROR, "redis insert error because :");
        cout << reply->str;
        return false;
    }
    return true;
}

std::string Redis::Get(std::string& key) {
    if (!mRedis) {
        LOG(ERROR, "redis not init now");
        return "";
    }
    redisReply* reply = (redisReply*)redisCommand(mRedis, "GET %s",
            key.c_str());
    if (!reply) {
        LOG(ERROR, "dont have this word");
        return "";
    }
    return reply->str;
}
END_NAMESPACE;
