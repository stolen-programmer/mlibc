
#pragma once

typedef struct _recycle *Recycle;
typedef struct _recycle {

    void *_ptr;
    void *(*mem)(Recycle);
    void (*recycle)(void *ptr);
} SRecycle;

Recycle recycle_init_from(void *mem, void *ptr);

