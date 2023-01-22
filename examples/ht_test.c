/*
 * Written by Nicolai H. Brand (lytix.dev) 2022-2023.
 * Example usage of the hashtable implementation found in nicc.
 * Licensed under GPLv3, see the LICENSE file for more info.
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define NICC_HT_IMPLEMENTATION
#include "../nicc.h"


void foo(struct ht_t *table)
{
    char *str = "this string is copied to the heap!";
    /* ht_set_alloc, ht_sset_alloc and ht_sset_alloc_T will copy the data over to the heap */
    ht_sset_alloc(table, "heap", str, strlen(str) + 1);
}

int main(void)
{
    struct ht_t *table = ht_alloc(32);

    char *key1 = "my key"; 
    char *val1 = "hello world!";
    ht_sset(table, key1, val1);
    assert(ht_sget(table, key1) == val1);

    /* example where value is a a struct */
    typedef struct {
        int a;
        char str[32];
    } my_struct;

    char *key2 = "my second key";
    my_struct mst = { .a = 10, .str = "hello" };
    ht_set(table, key2, strlen(key2) + 1, &mst);
    assert(memcmp((my_struct *)ht_sget(table, key2), &mst, sizeof(my_struct)) == 0);

    assert(ht_get_len(table) == 2);
    ht_srm(table, key1);
    assert(ht_get_len(table) == 1);

    void **res = ht_raw(table);
    assert(memcmp(res[0], &mst, sizeof(my_struct)) == 0);

    foo(table);
    char *str = ht_sget(table, "heap");
    assert(strcmp(str, "this string is copied to the heap!") == 0);
    ht_srm(table, "heap");

    ht_free(table);

    return 0;
}
