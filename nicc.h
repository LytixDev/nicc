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

#ifndef NICC_NICC_H
#define NICC_NICC_H

#include <stdlib.h>     // free, malloc, size_t type
#include <string.h>
#include <stdint.h>


/* darr functions */
struct darr_t *darr_malloc(void);
void darr_free(struct darr_t *da);
/*
 * appends the given value parameter to the end of the array.
 * automatically allocates more space in the array if necessary.
 */
void darr_append(struct darr_t *da, void *val);
/*
 * adds the given value parameter to the given index parameter position.
 */
void darr_set(struct darr_t *da, void *val, size_t idx);
/*
 * removes the value stored at the given index and shifts the
 * values to the right one down.
 */
void darr_rm(struct darr_t *da, size_t idk);
/*
 * removes and returns the last value stored in the array
 */
void *darr_pop(struct darr_t *da);
void *darr_get(struct darr_t *da, size_t idx);
size_t darr_get_size(struct darr_t *da);
/*
 * populates the raw parameter with pointers to the values stored
 * in the array and NULL terminates it. The raw void pointer array 
 * should be of size darr_get_size() + 1.
 */
void darr_raw(struct darr_t *da, void *raw[]);

/* ht functions */
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


#endif /* NICC_NICC_H */


#ifdef NICC_IMPLEMENTATION
#       define NICC_DARR_IMPLEMENTATION
#       define NICC_HT_IMPLEMENTATION
#       define NICC_STACK_IMPLEMENTATION
#endif

#if defined NICC_IMPLEMENTATION || defined NICC_DARR_IMPLEMENTATION || \
    defined NICC_HT_IMPLEMENTATION || defined NICC_STACK_IMPLEMENTATION

/* internal macros */
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, new_size) \
    (type *)nicc_internal_realloc(pointer, sizeof(type) * (new_size))


/* internal function definitions */
void *nicc_internal_realloc(void *ptr, size_t new_size)
{
    void *res = realloc(ptr, new_size);
    //TODO: better error handling
    if (res == NULL)
        exit(1);

    return res;
}

#endif /* any implementation */


#ifdef NICC_DARR_IMPLEMENTATION

/* darr implementation */
struct darr_t {
    void **data;
    size_t size;
    size_t cap;
};

struct darr_t *darr_malloc(void)
{
    struct darr_t *da = (struct darr_t *)malloc(sizeof(struct darr_t));

#ifdef DARR_STARTING_CAP
    da->cap = DARR_STARTING_CAP;
#else
    da->cap = GROW_CAPACITY(da->cap);
#endif

    da->size = 0;
    da->data = (void **)malloc(sizeof(void *) * da->cap);

    return da;
}

void darr_free(struct darr_t *da)
{
    free(da->data);
    free(da);
}

void darr_set(struct darr_t *da, void *val, size_t idx)
{
    if (da->size >= da->cap) {
        /* increase capacity */
        da->cap = GROW_CAPACITY(da->cap);
        da->data = GROW_ARRAY(void, da, da->cap);
    }

    da->data[idx] = val;
    /* only increase size if we are not overriding a pre-existing value */
    if (idx >= da->size)
        da->size++;
}

inline void darr_append(struct darr_t *da, void *val)
{
    darr_set(da, val, da->size);
}

void *darr_get(struct darr_t *da, size_t idx)
{
    if (idx >= da->size)
        return NULL;

    return da->data[idx];
}

void darr_rm(struct darr_t *da, size_t idx)
{
    if (idx > da->size || idx < 0)
        return;

    for (size_t i = idx + 1; i < da->size; i++)
        da->data[i - 1] = da->data[i];

    da->size--;
}

void *darr_pop(struct darr_t *da)
{
    void *item = da->data[da->size - 1];
    darr_rm(da, da->size - 1);
    return item;
}

inline size_t darr_get_size(struct darr_t *da)
{
    return da->size;
}

void darr_raw(struct darr_t *da, void *raw[da->size + 1])
{
    for (size_t i = 0; i < da->size; i++)
        raw[i] = da->data[i];

    da->data[da->size + 1] = NULL;
}

#endif /* DARR_IMPLEMENTATION */


#ifdef NICC_HT_IMPLEMENTATION

//#define HT_KEY_LIST /* if defined then ht_t will store a list of items stored per hash */

struct ht_item_t {
    void *key;
    size_t key_size;            /* total bytes stored in key */
    void *value;
    void (*free_func)(void *);  /* the free function used for freeing 'value' */
    struct ht_item_t *next;
};

struct ht_t {
    struct ht_item_t **items;
    size_t capacity;    /* bucket capacity */
#ifdef HT_KEY_LIST
    size_t *keys;       /* amount of items stored per hash */
#endif
};


static size_t hasher(const void *key, size_t key_size, size_t upper_bound)
{
    size_t hash = 0;

    for (size_t i = 0; i < key_size; i++)
	hash += (hash << 5) + ((int8_t *)key)[i];

    return hash % upper_bound;
}

struct ht_t *ht_malloc(size_t capacity)
{
    struct ht_t *ht = malloc(sizeof(struct ht_t));
    ht->capacity = capacity;
    ht->items = malloc(ht->capacity * sizeof(struct ht_item_t*));
#ifdef HT_KEY_LIST
    ht->keys = calloc(ht->capacity, sizeof(size_t));
#endif

    for (size_t i = 0; i < ht->capacity; i++)
        ht->items[i] = NULL;

    return ht;
}

void ht_free(struct ht_t *ht)
{
    struct ht_item_t *item;
    struct ht_item_t *prev;

    for (size_t i = 0; i < ht->capacity; i++) {
        item = ht->items[i];
        while (item != NULL) {
            free(item->key);

            if (item->free_func != NULL)
                item->free_func(item->value);

            free(item->value);
            prev = item;
            item = item->next;
            free(prev);
        }
    }
    free(ht->items);

#ifdef HT_KEY_LIST
    free(ht->keys);
#endif
    free(ht);
}

static struct ht_item_t *ht_item_add(const void *key, size_t key_size, const void *value,
                                     size_t val_size, void (*free_func)(void *))
{
    struct ht_item_t *ht_item = malloc(sizeof(struct ht_item_t));
    ht_item->key = malloc(key_size);
    memcpy(ht_item->key, key, key_size);

    ht_item->value = malloc(val_size);
    memcpy(ht_item->value, value, val_size);

    ht_item->next = NULL;
    ht_item->key_size = key_size;
    ht_item->free_func = free_func;
    return ht_item;
}

void ht_set(struct ht_t *ht, const void *key, size_t key_size, const void *value,
            size_t val_size, void (*free_func)(void *))
{
    size_t hash = hasher(key, key_size, ht->capacity);
    /* add hash to list of keys */
#ifdef HT_KEY_LIST
    ht->keys[hash] += 1;
#endif

    struct ht_item_t *found;
    struct ht_item_t *item = ht->items[hash];

    /* no ht_item means hash empty, insert immediately */
    if (item == NULL) {
        found = ht_item_add(key, key_size, value, val_size, free_func);
        ht->items[hash] = found;
        return;
    }

    struct ht_item_t *prev;

    /*
     * walk through each ht_item until either the end is
     * reached or a matching key is found
     */
    while (item != NULL) {
        if (key_size == item->key_size) {
            if (memcmp(key, item->key, key_size) == 0) {
                /* match found, replace value and free_func */
                free(item->value);
                item->value = malloc(val_size);
                memcpy(item->value, value, val_size);
                item->free_func = free_func;
                return;
            }
        }

        prev = item;
        item = prev->next;
    }

    /* end of chain reached without a match, add new */
    prev->next = ht_item_add(key, key_size, value, val_size, free_func);
}

void *ht_get(struct ht_t *ht, const void *key, size_t key_size)
{
    size_t hash = hasher(key, key_size, ht->capacity);
    struct ht_item_t *item = ht->items[hash];

    if (item == NULL)
        return NULL;

    while (item != NULL) {
        if (key_size == item->key_size)
            if (memcmp(key, item->key, key_size) == 0)
                return item->value;

        item = item->next;
    }

    /* reaching here means there were >= 1 items but no key match */
    return NULL;
}

struct ht_item_t *ht_geth(struct ht_t *ht, size_t hash)
{
    return ht->items[hash];
}

void ht_rm(struct ht_t *ht, const void *key, size_t key_size)
{
    size_t hash = hasher(key, key_size, ht->capacity);
    struct ht_item_t *item = ht->items[hash];

    if (item == NULL)
        return;

    struct ht_item_t *prev;
    int i = 0;

    while (item != NULL) {
        if (strcmp(item->key, key) == 0) {
            /* first ht_item and no next ht_item */
            if (item->next == NULL && i == 0)
                ht->items[hash] = NULL;

            /* first ht_item with a next ht_item */
            if (item->next != NULL && i == 0)
                ht->items[hash] = item->next;

            /* last ht_item */
            if (item->next == NULL && i != 0)
                prev->next = NULL;

            /* middle ht_item */
            if (item->next != NULL && i != 0)
                prev->next = item->next;

            /* free the deleted ht_item */
            free(item->key);
            free(item->value);
            free(item);

#ifdef HT_KEY_LIST
            ht->keys[hash] -= 1;
#endif

            return;
        }

        /* walk to next */
        prev = item;
        item = prev->next;
        i++;
    }
}

#endif /* NICC_HT_IMPLEMENTATION */
