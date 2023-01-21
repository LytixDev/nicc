/*
 * Written by Nicolai H. Brand (lytix.dev) 2023.
 * Example usage of the hashtable implementation found in nicc.
 * Licensed under GPLv3, see the LICENSE file for more info.
 */
#include <stdio.h>
#include <string.h>

#include "../ht.h"

int main(void)
{
    struct ht_t *ht = ht_alloc(64);
    char *hello = "hello world";
    ht_sset(ht, "first", hello);
    char *s = ht_sget(ht, "first");
    if (s != NULL)
        printf("%s\n", s);
}
