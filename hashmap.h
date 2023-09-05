/*
 *  Copyright (C) 2022-2023 Nicolai Brand
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
#ifndef NICC_HASHMAP_H
#define NICC_HASHMAP_H

#include <stdbool.h>

#include "common.h"

/* return codes for insert() function */
#define _HM_FULL 1
#define _HM_OVERRIDE 2
#define _HM_SUCCESS 3

#define HM_STARTING_BUCKETS_LOG2 3 // the amount of starting buckets
#define HM_BUCKET_SIZE 6
#define HM_OVERFLOW_SIZE 4
#define N_BUCKETS(log2) (1 << (log2))

/* hashmap */
/*
 * Quick note on the hashmap:
 * The hashmap impl. is a pretty standard dynamically growing hashmap. Every
 * bucket stores 6 entries by default. The aim of the hashmap implementation is
 * to be cache efficient and simple to use for any datatype and at any scale. It
 * takes major inspiration from the hashmap implemented in Go. NICC also
 * includes a fixed size linked-list hashtable called `ht`. The cache properties
 * of the fixed sized hashtable (ht) are likely to be noticably worse due to the
 * nature of linked list. It is also unlikely to scale particularly well.
 *
 * The hashmap implementation was originally written to be a part of a
 * collaborative project with a friend of mine. It can also be found here
 * amongst some examples and correctness tests:
 * https://github.com/DHPS-Solutions/dhps-lib
 */

struct hm_entry_t {
    void *key; // if NULL then entry is considered unused
    void *value;
    u32 key_size;
    u32 value_size;
    u8 hash_extra; // used for faster comparison
    u8 alloc_flag; // true if value is alloced
		   // if sizeof bool and _Bool > 3 then this struct would be of
		   // size 36 instead 32 due to struct padding. Therefore we use
		   // one byte to be safe we don't waste any memory space.
};

struct hm_bucket_t {
    struct hm_entry_t entries[HM_BUCKET_SIZE];
};

#ifdef NICC_TYPEDEF
typedef struct hashmap_t HashMap;
#endif /* NICC_TYPEDEF */

struct hashmap_t {
    struct hm_bucket_t *buckets;
    u8 size_log2;
    u32 len; // total items stored in the hashmap
    // #ifdef HASHMAP_THREAD_SAFE
    //     pthread_mutex_t lock;
    // #endif
};

void hashmap_init(struct hashmap_t *map);

void hashmap_free(struct hashmap_t *map);

void hashmap_put(struct hashmap_t *map, void *key, u32 key_size, void *value, u32 val_size,
		 bool alloc_flag);
#define hashmap_sput(map, key, value, val_size, alloc_flag) \
    hashmap_put(map, key, (strlen(key) + 1) * sizeof(char), value, val_size, alloc_flag)
#define hashmap_ssput(map, key, value, alloc_flag)                 \
    hashmap_put(map, key, (strlen(key) + 1) * sizeof(char), value, \
		(strlen(value) + 1) * sizeof(char), alloc_flag)

void *hashmap_get(struct hashmap_t *map, void *key, u32 key_size);
#define hashmap_sget(map, key) hashmap_get(map, key, (strlen(key) + 1) * sizeof(char))

bool hashmap_rm(struct hashmap_t *map, void *key, u32 key_size);
#define hashmap_srm(map, key) hashmap_rm(map, key, (strlen(key) + 1) * sizeof(char))

/*
 * The length of return_ptr must be at least sizeof(void *) * map->len bytes.
 * Anything less becomes UB.
 */
void hashmap_get_values(struct hashmap_t *map, void **return_ptr);

/*
 * The length of return_ptr must be at least sizeof(void *) * map->len bytes.
 * Anything less becomes UB.
 */
void hashmap_get_keys(struct hashmap_t *map, void **return_ptr);

#endif /* NICC_HASHMAP_H */
