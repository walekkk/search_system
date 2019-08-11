#ifndef SYNC_QUEUE_H
#define SYNC_QUEUE_H

#include <deque>
#include "./lock.h"

BEGIN_NAMESPACE(search_serever, util);

template<class T>
class SyncQueue {
public:
    SyncQueue();

    virtual ~SyncQueue();

    int32_t GetCapacity() const;
    
    void SetCapacity(int32_t);

    bool Push(const T& element);

    T GetFront();

    void PopFront();

    T GetAndPopFront();

    bool TryGetAndPopFront(T& element);

    bool IsEmpty();

    void WaitNotEmpty();

    int32_t GetSize();

    void WakeUp();

    void Clear();

public:
    static const int32_t DEF_WAIT_TIME = 1000000;
    static const int32_t DEF_CAPACITY = 10;

private:
    int32_t mCapacity;
    std::deque<T> mElement;
    ThreadCond mCond;
};

template<class T>
SyncQueue<T>::SyncQueue(): mCapacity(DEF_CAPACITY) {
}

template<class T>
SyncQueue<T>::~SyncQueue() {
}

template<class T>
int32_t SyncQueue<T>::GetCapacity() const {
    return mCapacity;
}

template<class T>
void SyncQueue<T>::SetCapacity(int32_t capacity) {
    mCapacity = capacity;
}

template<class T>
bool SyncQueue<T>::Push(const T& element) {
    ScopedLock lock(mCond);
    while (mElement.size() >= mCapacity) {
        mCond.Wait(DEF_WAIT_TIME);
    }
    mElement.push_back(element);
    mCond.Signal();
    return true;
}

template<class T>
T SyncQueue<T>::GetFront() {
    ScopedLock lock(mCond);
    const T& element = mElement.front();
    return element;
}

template<class T>
void SyncQueue<T>::PopFront() {
    ScopedLock lock(mCond);
    mElement.pop_front();
    mCond.Signal();
}

template<class T>
T SyncQueue<T>::GetAndPopFront() {
    ScopedLock lock(mCond);
    const T element = mElement.front();
    mElement.pop_front();
    mCond.Signal();
    return element;
}

template<class T>
bool SyncQueue<T>::TryGetAndPopFront(T& element) {
    ScopedLock lock(mCond);
    if (mElement.empty()) {
        return false;
    } else {
        element = mElement.front();
        mElement.pop_front();
        mCond.Signal();
    }
    return true;
}

template<class T>
void SyncQueue<T>::WaitNotEmpty() {
    ScopedLock lock(mCond);
    mCond.Wait(DEF_WAIT_TIME);
}

template<class T>
bool SyncQueue<T>::IsEmpty() {
    ScopedLock lock(mCond);
    return mElement.empty();
}
template<class T>
int32_t SyncQueue<T>::GetSize() {
    ScopedLock lock(mCond);
    return mElement.size();
}

template<class T>
void SyncQueue<T>::WakeUp() {
    ScopedLock lock(mCond);
    mCond.BroadCast();
}

template<class T>
void SyncQueue<T>::Clear() {
    ScopedLock lock(mCond);
    mElement.clear();
}

END_NAMESPACE;
#endif
