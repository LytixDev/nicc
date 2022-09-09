/*
 * Written by Nicolai H. Brand (lytix.dev) 2022.
 * Example usage of the dynamic array implementation found in nicc.
 * Licensed under GPLv3, see the LICENSE file for more info.
 */
#include <assert.h>
#include <string.h>

#include "../darr.h"


int main(void)
{
    darr *da = darr_malloc();
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
    darr_add(da, "hello world!", 0);
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
    /* create array that can store all data stored in da */
    size_t current_size = darr_get_size(da);
    char *raw[current_size];
    /* populates the 'raw' array with the data stored in da */
    darr_raw(da, (void **)raw);
    for (size_t i = 0; i < current_size; i++)
        assert(strcmp(raw[i], darr_get(da, i)) == 0);

    /* pop last item from array */
    char *item = darr_pop(da);
    /* know "hey" should be the last item */
    assert(strcmp(item, "hey") == 0);
    
    darr_free(da);
    return 0;
}
