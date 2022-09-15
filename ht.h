/*
 *  Hashtable implementation in C.
 *  Originally part of valery.
 *
 *  Inspired by:
 *  Python's dict implementation.
 *      <https://github.com/python/cpython/blob/main/Objects/dictobject.c>
 *  Engineer Man's hashtable implementation.
 *      <https://github.com/engineer-man/youtube/tree/master/077>
 *   
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

#ifndef NICC_HT_H
#define NICC_HT_H

#include <stdlib.h>

/* vars */

#define HT_KEY_LIST /* if defined then ht_t will store a list of items stored per hash */


/* types */

typedef struct ht_t ht;

typedef struct ht_item_t ht_item;


/* functions */

struct ht_t *ht_malloc(size_t capacity);

/* frees the entire ht and all items associated with it */
void ht_free(struct ht_t *ht);

/*
 * Allocates space for a new ht_item_t, computes the hash, and slots the 
 * item into the given 'ht_t *ht' hashtable. Frees and overrides previous
 * item if there is an item with the exact same key.
 *
 * mem_size argument should be the size of the value argument in bytes.
 *
 * If free_func argument is NULL, the value will be freed (later on) using
 * the standard free() function.
 * Provide a free_func argument for more complex types where free() is not
 * sufficient. The free_func should not free the type itself, but only its
 * members.
 * example
 *
 * void foo_free(void *p)
 * {
 *      struct foo *foo_object = (struct foo*)p;
 *      free(p->member1);
 *      free(p->member2);
 *      // DO NOT DO THIS: 'free(p)', it will be done by automatically by
 *      // the implementation
 * }
 */
void ht_set(struct ht_t *ht, const void *key, size_t key_size, const void *value,
            size_t mem_size, void (*free_func)(void *));

/* returns the value corresponding to the given key */
void *ht_get(struct ht_t *ht, const void *key, size_t key_size);

/* returns the first item stored with the given hash argument */
struct ht_item_t *ht_geth(struct ht_t *ht, size_t hash);

/* removes and frees the item the hashtable */
void ht_rm(struct ht_t *ht, const void *key, size_t key_size);

#endif /* NICC_HT_T */
