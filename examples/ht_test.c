/*
 * Written by Nicolai H. Brand (lytix.dev) 2022.
 * Example usage of the hashtable implementation found in nicc.
 * Licensed under GPLv3, see the LICENSE file for more info.
 */
#include <stdio.h>
#include <string.h>

#define NICC_HT_IMPLEMENTATION
#include "../nicc.h"


/* wrapper functions when key and val are string */
void ht_sset(struct ht_t *table, const char *key, const char *val)
{
    ht_set(table, key, strlen(key) + 1, val, strlen(val) + 1, NULL);
}

char *ht_sget(struct ht_t *table, const char *key)
{
    return ht_get(table, key, strlen(key) + 1);
}

int main(void)
{
    struct ht_t *table = ht_malloc(32);

    char *key1 = "my key"; 
    char *val1 = "hello world!";
    ht_sset(table, key1, val1);

    /* example where value is a a struct */
    typedef struct {
        int a;
        char str[32];
    } my_struct;
    char *key2 = "my second key";
    my_struct mst = { .a = 10, .str = "hello" };
    ht_set(table, key2, strlen(key2) + 1, &mst, sizeof(mst), NULL);

    /* results */
    char *res1 = ht_sget(table, key1);
    my_struct *res2 = ht_get(table, key2, strlen(key2) + 1);
    printf("%s = %s\n", val1, res1);
    printf("%s = %s\n", mst.str, res2->str);

    struct ht_item_t **raw = ht_raw(table);

    for (size_t i = 0; raw[i] != NULL; i++)
        printf("%s = %x\n", (char *)raw[i]->key, *(int *)raw[i]->value);

    free(raw);
    ht_free(table);
    
    return 0;
}
