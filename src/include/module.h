#include <stdint.h>

#ifndef _module_h_
#define _module_h_

#define IMPORT 2
#define LOCAL 1
#define NO_LABEL 0
#define EXPORT 3

typedef struct USAGE_LIST{
    struct USAGE_LIST *next;
    char               label[32];
    int32_t            value;
} USAGE_LIST;
    
#endif


