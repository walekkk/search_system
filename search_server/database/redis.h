#ifndef REDIS_H
#define REDIS_H

#include "../common/log.h"
#include <hiredis/redis.h>

BEGIN_NAMESPACE(search_server, database);

class Redis {
public:
    Redis();

    ~Redis();

    void Init();

    bool Push(std::string& key, std::string& value);

    std::string Get(std::string& key);

private:
    redisContext*   mRedis;
    int             mPort;
    char*           mIp;
};

END_NAMESPACE;
#endif
