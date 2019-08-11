#ifndef LOCK_H
#define LOCK_H

#include <pthread.h>
#include "./time_util.h"
#include "../common/common.h"

BEGIN_NAMESPACE(search_server, util);

class ThreadMutex {
public:
    ThreadMutex(const pthread_mutexattr_t* mta = NULL) {
        pthread_mutex_init(&mMutex, mta);
    }

    virtual ~ThreadMutex() {
        pthread_mutex_destroy(&mMutex);
    }

    int Lock() {
        return pthread_mutex_lock(&mMutex);
    }

    int TryLock() {
        return pthread_mutex_trylock(&mMutex);
    }

    int UnLock() {
        return pthread_mutex_unlock(&mMutex);
    }

private:
    ThreadMutex(const ThreadMutex&);

    ThreadMutex& operator = (const ThreadMutex&);

protected:
    pthread_mutex_t mMutex;
};

class ThreadCond: public ThreadMutex {
public:
    ThreadCond() {
        pthread_cond_init(&mCond, NULL);
    }

    virtual ~ThreadCond() {
        pthread_cond_destroy(&mCond);
    }

    int Wait(int64_t usec = 0) {
        int ret = 0;
        if (usec <= 0) {
            ret = pthread_cond_wait(&mCond, &mMutex);
        } else {
            timespec ts = TimeUtil::GetTimeSpec(usec);
            ret = pthread_cond_timedwait(&mCond, &mMutex, &ts);
        }
        return ret;
    }

    int Signal() {
        return pthread_cond_signal(&mCond);
    }

    int BroadCast() {
        return pthread_cond_broadcast(&mCond);
    }

protected:
    pthread_cond_t mCond;
};

class ScopedLock {
public:
    explicit ScopedLock(ThreadMutex& lock): mLock(lock) {
        int ret = mLock.Lock();
        assert(ret == 0); (void) ret;
    }

    virtual ~ScopedLock() {
        int ret = mLock.UnLock();
        assert(ret == 0); (void) ret;
    }

private:
    ScopedLock(const ScopedLock&);
    
    ScopedLock& operator= (const ScopedLock&);

private:
    ThreadMutex& mLock;
};

class ReadWriteLock {
public:
    enum Mode {
        PREFER_READER,
        PREFER_WRITER
    };

    ReadWriteLock(Mode mode = PREFER_READER) {
        pthread_rwlockattr_t attr;
        pthread_rwlockattr_init(&attr);
        switch(mode) {
            case PREFER_WRITER:
                pthread_rwlockattr_setkind_np(&attr,
                    PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
                break;
            case PREFER_READER:
                pthread_rwlockattr_setkind_np(&attr,
                    PTHREAD_RWLOCK_PREFER_READER_NP);
                break;
            default:
                break;
        }
        pthread_rwlock_init(&mLock, &attr);
    }

    virtual ~ReadWriteLock() {
        pthread_rwlock_destroy(&mLock);
    }

private:
    ReadWriteLock(const ReadWriteLock&);

    ReadWriteLock& operator = (const ReadWriteLock&);

public:
    int RdLock() {
        return pthread_rwlock_rdlock(&mLock);
    }

    int WrLock() {
        return pthread_rwlock_wrlock(&mLock);
    }

    int TryRdLock() {
        return pthread_rwlock_tryrdlock(&mLock);
    }

    int TryWrLock() {
        return pthread_rwlock_trywrlock(&mLock);
    }

    int UnLock() {
        return pthread_rwlock_unlock(&mLock);
    }

private:
    pthread_rwlock_t mLock;
};

class ScopedReadLock {
private:
    ScopedReadLock(const ScopedReadLock&);

    ScopedReadLock& operator = (const ScopedReadLock&);

public:
    explicit ScopedReadLock(ReadWriteLock& lock): mLock(lock) {
        int ret = mLock.RdLock();
        assert(ret == 0); (void) ret;
    }

    virtual ~ScopedReadLock() {
        int ret = mLock.UnLock();
        assert(ret == 0); (void) ret;
    }

private:
    ReadWriteLock& mLock;
};

class ScopedWriteLock {
private:
    ScopedWriteLock(const ScopedWriteLock&);

    ScopedWriteLock& operator = (const ScopedWriteLock&);

public:
    explicit ScopedWriteLock(ReadWriteLock& lock): mLock(lock) {
        int ret = mLock.WrLock();
        assert(ret == 0); (void) ret;
    }

    virtual ~ScopedWriteLock() {
        int ret = mLock.UnLock();
        assert(ret == 0); (void) ret;
    }

private:
    ReadWriteLock& mLock;
};

class ScopedReadWriteLock {
private:
    ScopedReadWriteLock(const ScopedReadWriteLock&);

    ScopedReadWriteLock& operator = (const ScopedReadWriteLock&);

public:
    explicit ScopedReadWriteLock(ReadWriteLock& lock, const char mode)
            :mLock(lock), mMode(mode) {
        if (mMode == 'r' || mMode == 'R') {
           int ret = mLock.RdLock();
           assert(ret == 0); (void) ret;
        } else if (mMode == 'w' || mMode == 'W') {
            int ret = mLock.WrLock();
            assert(ret == 0); (void) ret;
        } 
    }

    virtual ~ScopedReadWriteLock() {
        if (mMode == 'r' || mMode == 'R' ||
                mMode == 'w' || mMode == 'W') {
            int ret = mLock.UnLock();
            assert(ret == 0); (void) ret;
        }
    }

private:
    ReadWriteLock& mLock;
    char mMode;
};

END_NAMESPACE;
#endif
