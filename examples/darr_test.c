/*
 * Written by Nicolai H. Brand (lytix.dev) 2022.
 * Example usage of the dynamic array implementation found in nicc.
 * Licensed under GPLv3, see the LICENSE file for more info.
 */
#include <stdio.h>

#include "../darr.h"


int main(void)
{
    darr *da = darr_malloc();
    /*
     * also valid:
     * struct darr_t *da = darr_malloc();
     */
    int a = 10, b = 42, c = 11;
    char *str = "hello", *str2 = "world!";

    darr_append(da, &a);
    darr_append(da, &b); 
    darr_append(da, &c);
    darr_append(da, str);
    darr_append(da, str2);

    /* expect "hello" */
    char *result = (char *)darr_get(da, 3);
    printf("expect '%s', got '%s'\n", str, result);

    /* override "hello" */
    darr_add(da, "hello world!", 3);
    result = (char *)darr_get(da, 3);
    printf("expect 'hello world!', got '%s'\n", result);

    /*
     * remove "hello world" at index 3
     * this will shift index 4, which is "world!" into index 3
     */
    darr_rm(da, 3);
    result = (char *)darr_get(da, 3);
    printf("expect 'world!', got '%s'\n", result);

    darr_free(da);

    return 0;
}
