#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include <sys/time.h>
#include "./common.h"

#define INFO 0
#define ERROR 1
#define DEBUG 2

int64_t GetTimeStamp() {
    struct timeval mTime;
    gettimeofday(&mTime, NULL);
    return mTime.tv_sec;
}

std::string GetLogLevel(int32_t level) {
    switch(level) {
        case 0:
            return "INFO";
        case 1:
            return "ERROR";
        case 2:
            return "DEBUG";
        default:
            return "UNKNOW";
    }
}

void LOG(int level, std::string massage, 
         std::string file, int line) {
    std::cout << "【 " << GetTimeStamp() << ", " 
        << GetLogLevel(level) << ", " << file << " : " 
        << line << " 】" << massage << std::endl;
}
#define LOG(level, massage) LOG(level, massage, __FILE__, __LINE__)

#endif
