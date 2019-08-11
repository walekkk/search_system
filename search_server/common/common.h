#ifndef COMMON_H
#define COMMON_H

#define BEGIN_NAMESPACE(x, y) namespace x { namespace y {
#define END_NAMESPACE } }
#define USE_NAMESPACE(x, y) using namespace x::y

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>

#define DELETE_AND_SET_NULL(ptr)        \
    if (ptr != NULL) {                  \
        delete ptr;                     \
        ptr = NULL;                     \
    }                                   

#define ARRAY_DELETE_AND_SET_NULL(x)    \
    if ((x) != NULL) {                  \
        delete[] (x);                   \
        (x) = NULL;                     \
    }                                   

#define FREE_AND_SET_NULL(ptr)          \
        if (ptr != NULL) {              \
            free(ptr);                  \
            ptr = NULL;                 \
        }

#endif
