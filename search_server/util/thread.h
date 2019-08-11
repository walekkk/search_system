#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <memory>
#include <tr1/memory>
#include <tr1/functional>
#include "../common/log.h"
#include <memory>

BEGIN_NAMESPACE(search_server, util);

class Thread;
typedef std::tr1::shared_ptr<Thread> ThreadPtr;

struct ThreadFunctionWrapperArg {
    std::tr1::function<void ()> threadFunction;
};

class Thread {
private:
    static void* ThreadWrapperFunction(void* arg) {
        std::tr1::shared_ptr<ThreadFunctionWrapperArg>
            p(static_cast<ThreadFunctionWrapperArg*>(arg));
        p->threadFunction();
        return NULL;
    }

    Thread() {mId = 0;}

public:
    static ThreadPtr CreateThread(const std::tr1::function<void ()>& threadFunction) {
        ThreadPtr threadPtr(new Thread);
        ThreadFunctionWrapperArg* arg = new ThreadFunctionWrapperArg;
        arg->threadFunction = threadFunction;

        int rtn = pthread_create(&threadPtr->mId, NULL, &Thread::ThreadWrapperFunction, arg);

        if (rtn != 0) {
            delete arg;
            threadPtr->mId = 0;
            threadPtr.reset();
            LOG(ERROR, "create thread error");
        }
        return threadPtr;
    }

    pthread_t GetId() const {return mId;}

    void Join() {
        if (mId) {
            int ret = pthread_join(mId, NULL);
            (void) ret; assert(ret == 0);
        }
        mId = 0;
    }

    ~Thread() {
        Join();
    }

private:
    pthread_t mId;
};

END_NAMESPACE;
#endif
