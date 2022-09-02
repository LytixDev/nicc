#include <stdio.h>
#include <string.h>

#include "../ht.h"


void ht_str_set(ht *table, const char *key, const char *val)
{
    ht_set(table, key, strlen(key) + 1, val, strlen(val) + 1, NULL);
}

char *ht_str_get(ht *table, const char *key)
{
    return ht_get(table, key, strlen(key) + 1);
}

int main ()
{
    ht *table = ht_malloc(32);
    /*
     * also valid:
     * struct ht_t *table = ht_malloc(32);
     */

    char *key = "my key"; 
    char *val = "hello world!";

    ht_str_set(table, key, val);
    char *res = ht_str_get(table, key);
    printf("%s = %s\n", val, res);
    ht_free(table);
    
    return 0;
}
