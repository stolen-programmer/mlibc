
#pragma once

#define check_null(exp, r) check_log(exp != NULL, #exp " is NULL", r)

#define check_log(exp, msg, r)                                                 \
    if (!(exp)) {                                                              \
      printf("%s\n\tat %s(%s:%d)\n", msg, __FUNCTION__, __FILE__,              \
             __LINE__ - 1);                                                    \
             return r;\
    }
