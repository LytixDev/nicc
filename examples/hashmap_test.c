/*
 * Written by Nicolai H. Brand (lytix.dev) 2022-2023.
 * Example usage of the hashmap implementation found in nicc.
 * Licensed under GPLv3, see the LICENSE file for more info.
 */
#include <string.h>
#include <assert.h>

#define NICC_HASHMAP_IMPLEMENTATION
#include "../nicc.h"

void foo(struct hashmap_t *map)
{
    hashmap_ssput(map, "key", "value", true);
}

int main()
{
    struct hashmap_t map;
    hashmap_init(&map);

    foo(&map);

    void *v = hashmap_sget(&map, "key");

    assert(strcmp((char *)v, "value") == 0);
    hashmap_free(&map);
}
