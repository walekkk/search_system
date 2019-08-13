#ifndef SEARCH_SERVER_H
#define SEARCH_SERVER_H

#include <evhttp.h>
#include <event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/event.h>
#include "../common/log.h"
#include "../strategy/insert_work.h"
#include "../strategy/search_work.h"

BEGIN_NAMESPACE(search_server, service);

class SearchServer {
public:
    SearchServer();

    virtual ~SearchServer();

    bool Init();

    bool Start();

    bool Stop();

private:
    void CutString(std::string param, std::string& data1, std::string& data2);

    void EventCallBack(struct evhttp_request* request, void* arg);

private:
    bool                                        mStart;
    struct evhttp*                              mServer;
    struct event_base*                          mBase;
    strategy::InsertWork*                       mInsert;
    strategy::SearchWork*                       mSearch;
    util::ThreadPtr                             mThread;
    std::vector<util::ThreadPtr>                mThreadVector;
    util::SyncQueue<struct evhttp_request*>     mBlockingQueue;
}

END_NAMESPACE;
#endif
