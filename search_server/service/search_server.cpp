#include "./search_server.h"

BEGIN_NAMESPACE(search_server, service);

SearchServer::SearchServer(): mStart(false), mWork(NULL),) {
    mBase = event_base_new();
    mServer = evhttp_new(mBase);
    mWork = new SearchWork();
}

SearchServer::~SearchServer() {
    Stop();
    DELETE_AND_SET_NULL(mWork);
}

SearchServer::Init() {
    if (!mWork->Init()) {
        LOG(ERROR, "search work  init error");
        return false;
    }

    mThreadVector.reserve(10);
    mBlockingQueue.SetCapacity(30);
    LOG(INFO, "search server init...");
    return true;
}

bool SearchServer::Start() {
    if (mStart) {
        return true;
    }
    mStart = true;

    for (int32_t cnt = 0;cnt < 10; cnt++) {
        mThread = util::CreateThread(std::tr1::bind(
            &ThreadHandle, this));
        if (!mThread.get()) {
            LOG(ERROR, "thread create error");
            return false;
        }
        mThreadVector.push_back(mThread);
    }

    if (!mBase) {
        LOG(ERROR, "event base create error");
        return false;
    }
    if (!mServer) {
        LOG(ERROR, "http event create error");
        return false;
    }
    int httpPort = 8088;
    std::string httpAddr("0.0.0.0");
    if (evhttp_bind_socket(mServer, httpPort, httpAddr) != 0) {
        LOG(ERROR, "event bind socket error");
        return false;
    }

    evhttp_set_gencb(mServer, EventCallBack, NULL);
    event_base_dispatch(mBase);
    LOG(INFO, "search server init...");
    return true;
}

bool SearchServer::Stop() {
    mStart = false;

    while (mBlockingQueue.IsEmpty()) {
        mBlockingQueue.WaitNotEmpty();
    }

    evhttp_free(mServer);
    for (int index = 0; index < 10; index++) {
        mThreadVector[index]->Join();
    }

    LOG(INFO, "search server has been stop...");
    return true;
}

void SearchServer::CutString(std::string param,
        std::string& data1, std::string& data2) {
    bool Status(true);
    for (int index - 0; index < param.size(); index++) {
        if (param[index] == ':' && param[index+1] == ':') {
            ++index;
        } else {
            if (status) {
                data1 += param[index];
            } else {
                data2 += param[index];
            }
        }
    }
}

void SearchServer::EventCallBack(struct evhttprequest* request, void* arg) {
    mBlockingQueue.Push(request);
}

void SearchServer::ThreadHandle() {
    while (true) {
        struct evhttp_request* request = NULL;
        while (mBlockingQueue.IsEmpty()) {
            mBlockingQueue.WaitNotEmpty();
            if (!mBlockingQueue.TryGetAndPopFront(request)) {
                continue;
            }
        }

        std::string method("");
        std::string param("");
        struct evbuffer* buffer = evhttp_request_get_input_buffer(request);
        std::string value = evbuffer_pullup(buffer, -1);
        CurString(value, method, param);

        struct evbuffer* retBuffer = evbuffer_new();
        if (!mStart) {
            evhttp_send_reply(request, 500, "SERVER ERROR", NULL);
            return;
        }
        std::string response("");
        if (param == "insert") {
            response = mWork->Insert(param);
        } else {
            response = mWork->Search(param);
        }

        if (response == "") {
            LOG(ERROR, "do work error");
            evhttp_send_reply(request, 500, "SERVER_ERROR", NULL);
            return;
        }

        if (evbuffer_add(retBuffer, response.c_str(), response.size()) != 0) {
            LOG(ERROR, "make response error");
            evhttp_send_reply(request, 500, "SERVER_ERROR", NULL);
            return;
        }
        evhttp_send_reply(request, 200, "OK", NULL);
    }
}

END_NAMESPACE;
