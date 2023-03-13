/*
 * Written by Nicolai H. Brand (lytix.dev) 2022.
 * Example usage of the dynamic array implementation found in nicc.
 * Licensed under GPLv3, see the LICENSE file for more info.
 */
#include <assert.h>
#include <string.h>

#define NICC_DARR_IMPLEMENTATION
#include "../nicc.h"


int main(void)
{
    struct darr_t *da = darr_malloc();
    /*
     * also valid:
     * struct darr_t *da = darr_malloc();
     */
    char *str = "hello", *str2 = "world!";
    darr_append(da, str);
    darr_append(da, str2);

    /* expect "hello" */
    char *result = (char *)darr_get(da, 0);
    assert(strcmp(str, result) == 0);

    /* override "hello" */
    darr_set(da, "hello world!", 0);
    result = (char *)darr_get(da, 0);
    assert(strcmp("hello world!", result) == 0);

    /*
     * remove "hello world" at index 0
     * this will shift index 1, which is "world!" into index 0
     */
    size_t size_pre_rm = darr_get_size(da);
    darr_rm(da, 0);
    size_t size_post_rm = darr_get_size(da);
    result = (char *)darr_get(da, 0);
    assert(size_pre_rm == size_post_rm + 1);

    /* add another value */
    darr_append(da, "hey");

    /* pop last item from array */
    char *item = darr_pop(da);
    /* know "hey" should be the last item */
    assert(strcmp(item, "hey") == 0);
    /* pop "world!" */
    darr_pop(da);

    darr_append(da, "first");
    darr_append(da, "second");
    darr_append(da, "third");
    darr_rmv(da, "first", strlen("first") + 1);
    char *r = darr_get(da, 0);
    assert(strcmp(r, "second") == 0);
    
    darr_free(da);
    return 0;
}
