#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <sys/time.h>
#include "../common/common.h"

BEGIN_NAMESPACE(search_server, util);

class TimeUtil {
public:
    static int64_t CurrentTime();

    static int64_t GetTime(int64_t usecOffset = 0);

    static timespec GetTimeSpec(int64_t usecOffset = 0);
};

int64_t TimeUtil::CurrentTime() {
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return (tval.tv_sec * 1000000LL + tval.tv_usec);
}

int64_t TimeUtil::GetTime(int64_t usecOffset) {
    return CurrentTime() + usecOffset;
}

timespec TimeUtil::GetTimeSpec(int64_t usecOffset) {
    timespec tspec;
    int64_t uTime = GetTime(usecOffset);
    tspec.tv_sec = uTime/1000000;
    tspec.tv_nsec = (uTime % 1000000) * 1000;
    return tspec;
} 

END_NAMESPACE;
#endif
