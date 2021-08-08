
#include <recycle/recycle.h>
#include <log/log.h>

#include <stdio.h>

void *recycle_mem(Recycle recycle) {
    check_null(recycle, NULL)

    return recycle->_ptr;
}

void recycle_init_fun(Recycle recycle) {

    check_null(recycle, )

    recycle->mem = recycle_mem;
    recycle->recycle = NULL;
}

Recycle recycle_init_from(void *mem, void *ptr) {
    check_null(mem, NULL)
    check_null(ptr, NULL)

    Recycle self = mem;

    self->_ptr = ptr;

    recycle_init_fun(self);

    return self;

}
