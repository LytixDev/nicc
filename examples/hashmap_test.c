/*
 *  Copyright (C) 2022 Nicolai Brand
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "../hashmap.h"

typedef struct {
    int a;
    double b;
} Tuple;

bool tuple_eq(Tuple *a, Tuple *b)
{
    return a->a == b->a && a->b == b->b;
}


void foo(struct hashmap_t *map)
{
    hashmap_ssput(map, "key", "value", true);
}

void test_get_values_and_keys()
{
    struct hashmap_t map;
    hashmap_init(&map);

    Tuple a = { .a = 1, .b = 1.1 };
    Tuple b = { .a = 2, .b = 1.2 };
    Tuple c = { .a = 3, .b = 1.3 };
    hashmap_sput(&map, "key1", &a, sizeof(Tuple), true);
    hashmap_sput(&map, "key2", &b, sizeof(Tuple), true);
    hashmap_sput(&map, "key3", &c, sizeof(Tuple), true);

    /* sizeof(Tuple *) == sizeof(void *) */
    Tuple **ret_ptr = malloc(sizeof(Tuple *) * map.len);
    hashmap_get_values(&map, (void **)ret_ptr);

    Tuple *A = ret_ptr[0];
    Tuple *B = ret_ptr[1];
    Tuple *C = ret_ptr[2];

    assert(tuple_eq(A, &a) == true);
    assert(tuple_eq(B, &b) == true);
    assert(tuple_eq(C, &c) == true);

    /* get keys */
    char **ret_ptr_keys = malloc(sizeof(char *) * map.len);
    hashmap_get_keys(&map, (void **)ret_ptr_keys);

    char *key1 = ret_ptr_keys[0];
    char *key2 = ret_ptr_keys[1];
    char *key3 = ret_ptr_keys[2];

    assert(strcmp("key1", key1) == 0);
    assert(strcmp("key2", key2) == 0);
    assert(strcmp("key3", key3) == 0);

    free(ret_ptr);
    free(ret_ptr_keys);
    hashmap_free(&map);
}

int main()
{
    test_get_values_and_keys();

    struct hashmap_t map;
    hashmap_init(&map);

    foo(&map);

    void *v = hashmap_sget(&map, "key");

    assert(strcmp((char *)v, "value") == 0);
    hashmap_free(&map);
}
