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

#ifndef NICC_NICC_H
#define NICC_NICC_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


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

void **darr_raw_ret(struct darr_t *da);


/* ht functions */
struct ht_t *ht_alloc(size_t capacity);
/* frees the entire ht and all items associated with it */
void ht_free(struct ht_t *ht);

void ht_set(struct ht_t *ht, void *key, size_t key_size, void *value);
#define ht_sset(a, b, c) ht_set((a), (b), strlen(b) + 1, (c))

void *ht_get(struct ht_t *ht, void *key, size_t key_size);
#define ht_sget(a, b) ht_get(a, b, strlen(b) + 1)

void ht_rm(struct ht_t *ht, void *key, size_t key_size);
#define ht_srm(a, b) ht_rm(a, b, strlen(b) + 1)

/* returns a NULL terminated array of all nodes in th*/
void **ht_raw(struct ht_t *ht);


/* heapq functions */
/* compares priority of items in heapqueue. I:E: is a > b ? */
typedef bool (nicc_hq_compare_func)(void *a, void *b);
struct heapq_t *heapq_malloc(nicc_hq_compare_func *cmp);
void heapq_free(struct heapq_t *hq);
void heapq_push(struct heapq_t *hq, void *item);
void *heapq_get(struct heapq_t *hq, int idx);
/* 
 * returns and removes the item at the top of the heap queue.
 * note: remember to free() the popped item after use if it was malloced before pushing into
 * the heapq.
 */
void *heapq_pop(struct heapq_t *hq);


#endif /* NICC_NICC_H */


#ifdef NICC_IMPLEMENTATION
#       define NICC_DARR_IMPLEMENTATION
#       define NICC_HT_IMPLEMENTATION
#       define NICC_STACK_IMPLEMENTATION
#       define NICC_HEAPQ_IMPLEMENTATION
#endif

#if defined NICC_IMPLEMENTATION || defined NICC_DARR_IMPLEMENTATION || \
    defined NICC_HT_IMPLEMENTATION || defined NICC_STACK_IMPLEMENTATION || \
    defined NICC_HEAPQ_IMPLEMENTATION

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
    da->cap = 0;
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
        da->data = GROW_ARRAY(void *, da, da->cap);
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
    if (idx > da->size)
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

void **darr_raw_ret(struct darr_t *da)
{
    return da->data;
}

#endif /* DARR_IMPLEMENTATION */


#ifdef NICC_HT_IMPLEMENTATION

struct ht_item_t {
    void *key;
    size_t key_size;            /* total bytes stored in key */
    void *value;
    uint8_t hash_extra;
    struct ht_item_t *next;
};

struct ht_t {
    struct ht_item_t **buckets;
    size_t bucket_capacity;     /* bucket capacity */
    size_t len;                 /* total amount of item stored */
};


static size_t hash_func(const void *key, size_t key_size)
{
    size_t hash = 0;

    for (size_t i = 0; i < key_size; i++)
	hash += (hash << 5) + ((int8_t *)key)[i];

    return hash;
}

struct ht_t *ht_alloc(size_t capacity)
{
    struct ht_t *ht = malloc(sizeof(struct ht_t));
    ht->len = 0;
    ht->bucket_capacity = capacity;
    ht->buckets = malloc(ht->bucket_capacity * sizeof(struct ht_item_t *));

    for (size_t i = 0; i < ht->bucket_capacity; i++)
        ht->buckets[i] = NULL;

    return ht;
}

void ht_free(struct ht_t *ht)
{
    struct ht_item_t *item;
    struct ht_item_t *prev;

    for (size_t i = 0; i < ht->bucket_capacity; i++) {
        item = ht->buckets[i];
        while (item != NULL) {
            free(item->key);
            prev = item;
            item = item->next;
            free(prev);
        }
    }
    free(ht->buckets);
}

static struct ht_item_t *ht_item_add(void *key, size_t key_size, void *value, uint8_t extra)
{
    struct ht_item_t *ht_item = malloc(sizeof(struct ht_item_t));
    ht_item->key = malloc(key_size);
    ht_item->key_size = key_size;
    memcpy(ht_item->key, key, key_size);
    ht_item->value = value;
    ht_item->next = NULL;
    ht_item->hash_extra = extra;
    return ht_item;
}

void ht_set(struct ht_t *ht, void *key, size_t key_size, void *value)
{
    size_t hash_f = hash_func(key, key_size);
    size_t hash = hash_f % ht->bucket_capacity;
    uint8_t extra = (uint8_t)(hash >> (sizeof(size_t) - 8));

    struct ht_item_t *found;
    struct ht_item_t *item = ht->buckets[hash];

    /* no ht_item means hash empty, insert immediately */
    if (item == NULL) {
        found = ht_item_add(key, key_size, value, extra);
        ht->buckets[hash] = found;
        ht->len++;
        return;
    }

    struct ht_item_t *prev;

    /*
     * walk through each ht_item until either the end is
     * reached or a matching key is found
     */
    while (item != NULL) {
        if (key_size == item->key_size) {
            if (value == item->value) {
                /* match found, replace value */
                //free(item->value);
                item->value = value;
                return;
            }
        }

        prev = item;
        item = prev->next;
    }

    /* end of chain reached without a match, add new */
    prev->next = ht_item_add(key, key_size, value, extra);
    ht->len++;
}

void *ht_get(struct ht_t *ht, void *key, size_t key_size)
{
    size_t hash_f = hash_func(key, key_size);
    size_t hash = hash_f % ht->bucket_capacity;
    uint8_t extra = (uint8_t)(hash >> (sizeof(size_t) - 8));
    struct ht_item_t *item = ht->buckets[hash];

    while (item != NULL) {
        if (item->hash_extra == extra)
            if (memcmp(key, item->key, key_size) == 0)
                return item->value;

        item = item->next;
    }

    return NULL;
}

void ht_rm(struct ht_t *ht, void *key, size_t key_size)
{
    size_t hash_f = hash_func(key, key_size);
    size_t hash = hash_f % ht->bucket_capacity;
    uint8_t extra = (uint8_t)(hash >> (sizeof(size_t) - 8));

    struct ht_item_t *item = ht->buckets[hash];

    if (item == NULL)
        return;

    struct ht_item_t *prev;
    int i = 0;

    while (item != NULL) {
        if (item->hash_extra != extra)
            goto cont;

        if (memcmp(item->key, key, key_size) == 0) {
            /* first ht_item and no next ht_item */
            if (item->next == NULL && i == 0)
                ht->buckets[hash] = NULL;

            /* first ht_item with a next ht_item */
            if (item->next != NULL && i == 0)
                ht->buckets[hash] = item->next;

            /* last ht_item */
            if (item->next == NULL && i != 0)
                prev->next = NULL;

            /* middle ht_item */
            if (item->next != NULL && i != 0)
                prev->next = item->next;

            /* free the deleted ht_item */
            free(item->key);
            free(item);
            ht->len--;
            return;
        }

    cont:
        /* walk to next */
        prev = item;
        item = prev->next;
        i++;
    }
}

size_t ht_get_len(struct ht_t *ht)
{
    return ht->len;
}

void **ht_raw(struct ht_t *ht)
{
    void **raw = malloc(ht->len * (sizeof(void *) + 1));
    raw[ht->len] = NULL;
    size_t pos = 0;

    for (size_t i = 0; i < ht->bucket_capacity; i++) {
        struct ht_item_t *p = ht->buckets[i] ;
        while (p != NULL) {
            raw[pos++] = p->value;
            p = p->next;
        }
    }

    return raw;
}
#endif /* NICC_HT_IMPLEMENTATION */


#ifdef NICC_HEAPQ_IMPLEMENTATION

#define HEAPQ_STARTING_CAPACITY 32

#define heapq_left_child_idx(parent_idx) (parent_idx * 2 + 1)
#define heapq_right_child_idx(parent_idx) (parent_idx * 2 + 2)
#define heapq_parent_idx(child_idx) ((child_idx - 1) / 2)

#define heapq_has_left(idx, size) (heapq_left_child_idx(idx) < size)
#define heapq_has_right(idx, size) (heapq_right_child_idx(idx) < size

/* heap queue inspired by: https://docs.python.org/3/library/heapq.html */
struct heapq_t {
    void **items; 
    int size;
    int capacity;
    nicc_hq_compare_func *cmp;
};

static inline void *heapq_left_child(struct heapq_t *hq, int idx)
{
    return hq->items[heapq_left_child_idx(idx)];
}

static inline void *heapq_right_child(struct heapq_t *hq, int idx)
{
    return hq->items[heapq_right_child_idx(idx)];
}

static inline void *heapq_parent(struct heapq_t *hq, int idx)
{
    return hq->items[heapq_parent_idx(idx)];
}

static void heapq_swap(struct heapq_t *hq, int a, int b)
{
    void *tmp = hq->items[a];
    hq->items[a] = hq->items[b];
    hq->items[b] = tmp;
}

static void heapify_up(struct heapq_t *hq)
{
    int idx = hq->size - 1;
    int parent_idx = heapq_parent_idx(idx);
    /* keep "repearing" heap as long as parent is greater than child */
    while (parent_idx >= 0 && hq->cmp(hq->items[parent_idx], hq->items[idx])) {
        heapq_swap(hq, parent_idx, idx);
        /* walk upwards */
        idx = heapq_parent_idx(idx);
        parent_idx = heapq_parent_idx(idx);
    }
}

static void heapify_down(struct heapq_t *hq)
{
    int idx = 0;
    int min_idx;
    while (heapq_has_left(idx, hq->size)) {
        min_idx = heapq_left_child_idx(idx);
        if (heapq_has_right(idx, hq->size) && hq->cmp(hq->items[min_idx],
                                                      heapq_right_child(hq, idx))))
            min_idx = heapq_right_child_idx(idx);

        if (hq->cmp(hq->items[min_idx], hq->items[idx])) {
            break;
        } else {
            heapq_swap(hq, idx, min_idx);
            idx = min_idx;
        }
    }
}

void *heapq_get(struct heapq_t *hq, int idx)
{
    if (idx < 0 || idx >= hq->size)
        return NULL;

    return hq->items[idx];
}

void *heapq_pop(struct heapq_t *hq)
{
    struct node_t *item = heapq_get(hq, 0);
    if (item == NULL)
        return NULL;

    hq->items[0] = hq->items[--hq->size];
    heapify_down(hq);
    return item;
}

void heapq_push(struct heapq_t *hq, void *item)
{
    if (hq->size >= hq->capacity) {
        hq->capacity = GROW_CAPACITY(hq->capacity);
        hq->items = GROW_ARRAY(void *, hq, hq->capacity);
    }
    hq->items[hq->size++] = item;
    heapify_up(hq);
}

void heapq_free(struct heapq_t *hq)
{
    free(hq->items);
    free(hq);
}

struct heapq_t *heapq_malloc(nicc_hq_compare_func *cmp)
{
    struct heapq_t *hq = malloc(sizeof(struct heapq_t));
    hq->size = 0;
    hq->capacity = HEAPQ_STARTING_CAPACITY;
    hq->items = malloc(hq->capacity * sizeof(void *));
    hq->cmp = cmp;
    return hq;
}

#endif /* NICC_HEAPQ_IMPLEMENTATION */
