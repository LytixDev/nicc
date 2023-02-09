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
#include <assert.h>
#ifdef HASHMAP_THREAD_SAFE
#  include <pthread.h>
#endif


/* hashmap functions */
/*
 * Quick note on the hashmap:
 * The hashmap impl. is a pretty standard dynamically growing hashmap. Every bucket stores 6 entries
 * by default. The aim of the hashmap implementation is to be cache efficient and simple to use
 * for any datatype and at any scale. It takes major inspiration from the hashmap implemented in Go.
 * NICC also includes a fixed size linked-list hashtable called `ht`. The cache properties of the
 * fixed sized hashtable (ht) are likely to be noticably worse due to the nature of linked list. It
 * is also unlikely to scale particularly well.
 *
 * Lastly, the hashmap implementation was originally written to be a part of a collaborative
 * project with a friend of mine. It was originally licensed under the MIT license, but as the sole
 * authour of the source code, I license it under GPL3 per my own discretion.
 */

void hashmap_init(struct hashmap_t *map);

void hashmap_free(struct hashmap_t *map);

void hashmap_put(struct hashmap_t *map, void *key, size_t key_size, void *t, size_t ts);

void *hashmap_get(struct hashmap_t *map, void *key, size_t key_size);

bool hashmap_rm(struct hashmap_t *map, void *key, size_t key_size);

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
/* frees the entire ht and all values allocated with it */
void ht_free(struct ht_t *ht);

void ht_set(struct ht_t *ht, void *key, size_t key_size, void *value);
#define ht_sset(a, b, c) ht_set(a, b, (strlen(b) + 1) * sizeof(char), c)

void ht_set_alloc(struct ht_t *ht, void *key, size_t key_size, void *value, size_t val_size);
#define ht_sset_alloc(a, b, c, d) ht_set_alloc(a, b, (strlen(b) + 1) * sizeof(char), c, d)
#define ht_sset_alloc_T(a, b, c, T) ht_set_alloc(a, b, (strlen(b) + 1) * sizeof(char), c, sizeof(T))

void *ht_get(struct ht_t *ht, void *key, size_t key_size);
#define ht_sget(a, b) ht_get(a, b, (strlen(b) + 1) * sizeof(char))

void ht_rm(struct ht_t *ht, void *key, size_t key_size);
#define ht_srm(a, b) ht_rm(a, b, (strlen(b) + 1) * sizeof(char))

size_t ht_get_len(struct ht_t *ht);

/* returns a NULL terminated array of all values */
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
#       define NICC_HASHMAP_IMPLEMENTATION
#endif

#if defined NICC_IMPLEMENTATION || defined NICC_DARR_IMPLEMENTATION || \
    defined NICC_HT_IMPLEMENTATION || defined NICC_STACK_IMPLEMENTATION || \
    defined NICC_HEAPQ_IMPLEMENTATION || defined NICC_HASHMAP_IMPLEMENTATION

/* internal macros */
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, new_size) \
    (type *)nicc_internal_realloc((pointer), sizeof(type) * (new_size))


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

#ifdef NICC_HASHMAP_IMPLEMENTATION

//#define HASHMAP_THREAD_SAFE
#define HASHMAP_BUCKET_SIZE 6
#define HASHMAP_STARTING_OVERFLOW_BUCKET_SIZE 4
#define HASHMAP_STARTING_BUCKETS_log2 3
#define N_BUCKETS(log2) (1 << (log2))

struct hashmap_entry_t {
    void *value;
    void *key;
    size_t key_size;
    uint8_t hash_extra;         // used for faster comparison
                                // the common case is that the bytes are not equal
};

struct overflow_bucket_t {
    // NOTE: this is really just a dynamic array.
    struct hashmap_entry_t *entries;
    uint8_t size, capacity;
};

struct bucket_t {
    struct hashmap_entry_t entries[HASHMAP_BUCKET_SIZE];   // 6 direct items. Spatial locality bros.
    struct overflow_bucket_t *overflow;
};

struct hashmap_t {
    struct bucket_t *buckets;
    uint32_t size_log2;
    size_t len;         // total items stored in the hashmap
#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_t lock;
#endif
};

static uint32_t hash_func(void *t, size_t ts)
{
    //NOTE: gigahafting kok (legger dermed ikke så mye lit til det)
    uint32_t A = 1327217885;
    uint32_t k = 0;
    for (size_t i = 0; i < ts; i++)
	k += (k << 5) + ((int8_t *)t)[i];

    return k * A;
}

static inline uint8_t hash_extra(uint32_t hash)
{
    int mask = (1 << 8) - 1;
    return (uint8_t)(hash & mask);
}

static struct overflow_bucket_t *overflow_alloc()
{
    struct overflow_bucket_t *overflow = malloc(sizeof(struct overflow_bucket_t));
    overflow->entries = malloc(sizeof(struct hashmap_entry_t) * HASHMAP_STARTING_OVERFLOW_BUCKET_SIZE);
    overflow->size = 0;
    overflow->capacity = HASHMAP_STARTING_OVERFLOW_BUCKET_SIZE;
    return overflow;
}

static struct hashmap_entry_t *scan_overflow_for_match(struct overflow_bucket_t *overflow,
                                                       void *key, size_t key_size, uint8_t extra)
{
    for (uint8_t i = 0; i < overflow->size; i++) {
        if (overflow->entries[i].value == NULL)
            continue;
        if (overflow->entries[i].hash_extra != extra || overflow->entries[i].key_size != key_size)
            continue;

        if (memcmp(overflow->entries[i].key, key, key_size) == 0)
            return &overflow->entries[i];
    }

    return NULL;
}

static inline void insert_entry(struct hashmap_entry_t *entry, void *key, size_t key_size,
                                void *value, uint8_t extra)
{
    entry->key = key;
    entry->key_size = key_size;
    entry->value = value;
    entry->hash_extra = extra;
}

static void insert_into_overflow(struct overflow_bucket_t *overflow, void *key, size_t key_size,
                                 void *value, uint8_t extra)
{
    /* this should really never happen assuming the hash function spreads values evenly */
    if (overflow->size >= overflow->capacity) {
        //TODO: resize of overflow bucket or force resize entire map?
        exit(1);
    }

    struct hashmap_entry_t *found = &overflow->entries[overflow->size++];
    insert_entry(found, key, key_size, value, extra);
}

static bool insert_into_bucket(struct bucket_t *bucket, void *key, size_t key_size, void *value,
                               uint8_t extra)
{
    /*
     * Our hashmap implementation does not allow duplcate entries.
     * Therefore, we cannot simply find the first empty entry and set the new entry here, we have 
     * to make sure the entry we are inserting does not already exist somewhere else in the bucket.
     * If we find a matching entry, we simply override. If we do not find a matching entry, we
     * insert the new entry in the first found empty entry.
     */
    struct hashmap_entry_t *found = NULL;

    for (uint8_t i = 0; i < HASHMAP_BUCKET_SIZE; i++) {
        if (bucket->entries[i].value != NULL) {
            /* check if stored entry has the same key as entry we want to store */
            if (bucket->entries[i].hash_extra != extra || bucket->entries[i].key_size != key_size)
                continue;

            if (memcmp(bucket->entries[i].key, key, key_size) == 0) {
                insert_entry(&bucket->entries[i], key, key_size, value, extra);
                return false;
            } else {
                continue;
            }
        }

        if (found == NULL)
            found = &bucket->entries[i];
    }

    if (found == NULL) {
        if (bucket->overflow == NULL)
            bucket->overflow = overflow_alloc();
        insert_into_overflow(bucket->overflow, key, key_size, value, extra);
    } else {
        /* check overflow bucket for duplicate entry */
        if (bucket->overflow != NULL) {
            struct hashmap_entry_t *duplicate = scan_overflow_for_match(bucket->overflow, key,
                                                                        key_size, extra);
            insert_entry(duplicate != NULL ? duplicate : found, key, key_size, value, extra);
        } else {
            bucket->overflow = overflow_alloc();
            insert_into_overflow(bucket->overflow, key, key_size, value, extra);
        }
    }

    return true;
}

static void *get_from_bucket(struct bucket_t *bucket, void *key, size_t key_size, uint8_t extra)
{
    for (uint8_t i = 0; i < HASHMAP_BUCKET_SIZE; i++) {
        if (bucket->entries[i].value == NULL)
            continue;
        if (bucket->entries[i].hash_extra != extra || bucket->entries[i].key_size != key_size)
            continue;

        /* please compiler, do not reorder muh code ! */
        if (memcmp(bucket->entries[i].key, key, key_size) == 0)
            return bucket->entries[i].value;
    }

    /* value not found, check overflow bucket */
    if (bucket->overflow == NULL)
        return NULL;

    struct overflow_bucket_t *overflow = bucket->overflow;
    for (uint8_t i = 0; i < overflow->size; i++) {
        if (overflow->entries[i].value == NULL)
            continue;
        if (overflow->entries[i].hash_extra != extra || overflow->entries[i].key_size != key_size)
            continue;

        if (memcmp(overflow->entries[i].key, key, key_size) == 0)
            return overflow->entries[i].value;
    }

    return NULL;
}

static void hash_and_insert(struct bucket_t *new_buckets, uint32_t size_log2,
                            struct hashmap_entry_t *entry)
{
    uint32_t hash_full = hash_func(entry->key, entry->key_size);
    uint32_t hash = hash_full >> (32 - size_log2);
    uint8_t extra = hash_extra(hash_full);
    insert_into_bucket(&new_buckets[hash], entry->key, entry->key_size, entry->value, extra);
}

static void move_entries(struct hashmap_t *map, struct bucket_t *new_buckets)
{
    // why -1? because thats the old size
    for (uint32_t i = 0; i < N_BUCKETS(map->size_log2 - 1); i++) {
        struct bucket_t old_bucket = map->buckets[i];
        for (uint8_t j = 0; j < HASHMAP_BUCKET_SIZE; j++) {
            if (old_bucket.entries[j].value != NULL)
                hash_and_insert(new_buckets, map->size_log2, &old_bucket.entries[j]);
        }

        /* move all overflow entries as well */
        if (old_bucket.overflow != NULL) {
            for (uint8_t j = 0; j < old_bucket.overflow->size; j++) {
                if (old_bucket.overflow->entries[j].value != NULL)
                    hash_and_insert(new_buckets, map->size_log2, &old_bucket.overflow->entries[j]);
            }
        }
    }

    //TODO: this is not sufficient
    free(map->buckets);
    map->buckets = new_buckets;
}

void hashmap_init(struct hashmap_t *map)
{
#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_init(&map->lock, NULL);
#endif
    map->len = 0;
    map->size_log2 = HASHMAP_STARTING_BUCKETS_log2;

    map->buckets = malloc(sizeof(struct bucket_t) * N_BUCKETS(map->size_log2));
    struct bucket_t bucket;
    for (uint32_t i = 0; i < N_BUCKETS(map->size_log2); i++) {
        bucket = map->buckets[i];
        bucket.overflow = NULL;
        for (uint8_t i = 0; i < HASHMAP_BUCKET_SIZE; i++)
            bucket.entries[i].value = NULL;
    }
}

void hashmap_free(struct hashmap_t *map)
{
#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_lock(&map->lock);
#endif
    struct bucket_t bucket;
    for (uint32_t i = 0; i < N_BUCKETS(map->size_log2); i++) {
        bucket = map->buckets[i];
        if (bucket.overflow != NULL) {
            free(bucket.overflow->entries);
            free(bucket.overflow);
        }
    }

    free(map->buckets);
#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_unlock(&map->lock);
#endif
}

void hashmap_put(struct hashmap_t *map, void *key, size_t key_size, void *t, size_t ts)
{
#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_lock(&map->lock);
#endif
    double load_factor = (double)map->len / (N_BUCKETS(map->size_log2) * HASHMAP_BUCKET_SIZE);
    if (load_factor >= 0.75) {
        // NOTE: instead of moving all entries in one go, we can only move over some buckets
        //       and invalidate the rest and move them once necesary
        /* 
         * just reallocating the buckets is not really feasable since every entry has to be moved,
         * to its new bucket. So if the buckets are already filled with old invalid entries, the new
         * entries would not have any space.
         */
        map->size_log2++;
        assert(map->size_log2 < 32);
        struct bucket_t *new_buckets = malloc(sizeof(struct bucket_t) * N_BUCKETS(map->size_log2));
        for (uint32_t i = 0; i < N_BUCKETS(map->size_log2); i++) {
            struct bucket_t bucket = new_buckets[i];
            bucket.overflow = NULL;
            for (uint8_t i = 0; i < HASHMAP_BUCKET_SIZE; i++)
                bucket.entries[i].value = NULL;
        }

        move_entries(map, new_buckets);
    }

    uint32_t hash_full = hash_func(key, key_size);
    uint32_t hash = hash_full >> (32 - map->size_log2);
    uint8_t extra = hash_extra(hash_full);
    bool rc = insert_into_bucket(&map->buckets[hash], key, key_size, t, extra);
    if (rc)
        map->len++;

#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_unlock(&map->lock);
#endif
}

void *hashmap_get(struct hashmap_t *map, void *key, size_t key_size)
{
#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_lock(&map->lock);
#endif
    uint32_t hash_full = hash_func(key, key_size);
    uint32_t hash = hash_full >> (32 - map->size_log2);
    uint8_t extra = hash_extra(hash_full);
#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_unlock(&map->lock);
#endif
    return get_from_bucket(&map->buckets[hash], key, key_size, extra);
}

bool hashmap_rm(struct hashmap_t *map, void *key, size_t key_size)
{
#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_lock(&map->lock);
#endif
    uint32_t hash_full = hash_func(key, key_size);
    uint32_t hash = hash_full >> (32 - map->size_log2);
    uint8_t extra = hash_extra(hash_full);
    struct hashmap_entry_t *entry =  get_from_bucket(&map->buckets[hash], key, key_size, extra);
    if (entry == NULL || entry->value == NULL) {
#ifdef HASHMAP_THREAD_SAFE
        pthread_mutex_unlock(&map->lock);
#endif
        return false;
    }

    entry->value = NULL;
#ifdef HASHMAP_THREAD_SAFE
    pthread_mutex_unlock(&map->lock);
#endif
    return true;
}

#endif /* NICC_HASHMAP_IMPLEMENTATION */


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
        da->data = GROW_ARRAY(void *, da->data, da->cap);
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
    uint8_t hash_extra_and_alloc_flag;
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
            if (item->hash_extra_and_alloc_flag & 128)
                free(item->value);
            free(item->key);
            prev = item;
            item = item->next;
            free(prev);
        }
    }
    free(ht->buckets);
}

static inline uint8_t hash_extra(size_t hash)
{
    size_t mask = (1 << 7) - 1;
    return (uint8_t)hash & mask;
}

static struct ht_item_t *ht_item_add(void *key, size_t key_size, void *value, size_t val_size,
                                     uint8_t extra)
{
    struct ht_item_t *ht_item = malloc(sizeof(struct ht_item_t));
    ht_item->key = malloc(key_size);
    ht_item->key_size = key_size;
    memcpy(ht_item->key, key, key_size);
    ht_item->hash_extra_and_alloc_flag = extra;

    if (val_size == 0) {
        ht_item->value = value;
    } else {
        /* set alloc flag */
        ht_item->hash_extra_and_alloc_flag |= 128; /* 0b10000000 */
        ht_item->value = malloc(val_size);
        memcpy(ht_item->value, value, val_size);
    }

    ht_item->next = NULL;
    return ht_item;
}

void ht_set_alloc(struct ht_t *ht, void *key, size_t key_size, void *value, size_t val_size)
{
    size_t hash_f = hash_func(key, key_size);
    size_t hash = hash_f % ht->bucket_capacity;
    uint8_t extra = hash_extra(hash);

    struct ht_item_t *found;
    struct ht_item_t *item = ht->buckets[hash];

    /* no ht_item means hash empty, insert immediately */
    if (item == NULL) {
        found = ht_item_add(key, key_size, value, val_size, extra);
        ht->buckets[hash] = found;
        ht->len++;
        return;
    }

    struct ht_item_t *prev;
    uint8_t item_extra;

    /*
     * walk through each ht_item until either the end is
     * reached or a matching key is found
     */
    while (item != NULL) {
        item_extra = item->hash_extra_and_alloc_flag;
        /* remove alloc flag if exists */
        item_extra &= (1 << 7) - 1;

        if (extra == item_extra) {
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
    prev->next = ht_item_add(key, key_size, value, val_size, extra);
    ht->len++;
}

void ht_set(struct ht_t *ht, void *key, size_t key_size, void *value)
{
    ht_set_alloc(ht, key, key_size, value, 0);
}

void *ht_get(struct ht_t *ht, void *key, size_t key_size)
{
    if (ht->len == 0)
        return NULL;

    size_t hash_f = hash_func(key, key_size);
    size_t hash = hash_f % ht->bucket_capacity;
    uint8_t extra = hash_extra(hash);

    struct ht_item_t *item = ht->buckets[hash];
    uint8_t item_extra;

    while (item != NULL) {
        item_extra = item->hash_extra_and_alloc_flag;
        /* remove alloc flag if exists */
        item_extra &= (1 << 7) - 1;

        if (item_extra == extra)
            if (memcmp(key, item->key, key_size) == 0)
                return item->value;

        item = item->next;
    }

    return NULL;
}

void ht_rm(struct ht_t *ht, void *key, size_t key_size)
{
    if (ht->len == 0)
        return ;

    size_t hash_f = hash_func(key, key_size);
    size_t hash = hash_f % ht->bucket_capacity;
    uint8_t extra = hash_extra(hash);

    struct ht_item_t *item = ht->buckets[hash];

    if (item == NULL)
        return;

    struct ht_item_t *prev;
    uint8_t item_extra;
    int i = 0;

    while (item != NULL) {
        item_extra = item->hash_extra_and_alloc_flag;
        /* remove alloc flag if exists */
        item_extra &= (1 << 7) - 1;

        if (item_extra != extra)
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
            if (item->hash_extra_and_alloc_flag & 128)
                free(item->value);
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
