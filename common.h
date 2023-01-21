#ifndef COMMON_H
#define COMMON_H
#include <stdlib.h>

/* internal macros */
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, new_size) \
    (type *)nicc_internal_realloc(pointer, sizeof(type) * (new_size))


/* internal function definitions */
void *nicc_internal_realloc(void *ptr, size_t new_size)
{
    void *res = realloc(ptr, new_size);
    //TODO: better error handling
    if (res == NULL)
        exit(1);

    return res;
}

#endif /* COMMON_H */
