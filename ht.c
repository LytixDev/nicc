/*
 *  Hashtable implementation in C.
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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "ht.h"


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

