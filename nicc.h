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
//#ifdef HASHMAP_THREAD_SAFE
//#  include <pthread.h>
//#endif

#ifndef u8
#  define u8 uint8_t
#endif
#ifndef u32
#  define u32 uint32_t
#endif

#define HM_STARTING_BUCKETS_LOG2 3 // the amount of starting buckets
#define HM_BUCKET_SIZE 6
#define HM_OVERFLOW_SIZE 4
#define N_BUCKETS(log2) (1 << (log2))

/* hashmap */
/*
 * Quick note on the hashmap:
 * The hashmap impl. is a pretty standard dynamically growing hashmap. Every bucket stores 6 entries
 * by default. The aim of the hashmap implementation is to be cache efficient and simple to use
 * for any datatype and at any scale. It takes major inspiration from the hashmap implemented in Go.
 * NICC also includes a fixed size linked-list hashtable called `ht`. The cache properties of the
 * fixed sized hashtable (ht) are likely to be noticably worse due to the nature of linked list. It
 * is also unlikely to scale particularly well.
 *
 * The hashmap implementation was originally written to be a part of a collaborative
 * project with a friend of mine. It can also be found here amongst some examples and correctness
 * tests: https://github.com/DHPS-Solutions/dhps-lib
 */

struct hm_entry_t {
    void *key;          // if NULL then entry is considered unused
    void *value;
    u32 key_size;
    u32 value_size;
    u8 hash_extra;      // used for faster comparison
    u8 alloc_flag;      // true if value is alloced
                        // if sizeof bool and _Bool > 3 then this struct would be of size 36
                        // instead 32 due to struct padding. Therefore we use one byte to be
                        // safe we don't waste any memory space.
};

struct hm_bucket_t {
    struct hm_entry_t entries[HM_BUCKET_SIZE];
};

struct hashmap_t {
    struct hm_bucket_t *buckets;
    u8 size_log2;
    u32 len;         // total items stored in the hashmap
//#ifdef HASHMAP_THREAD_SAFE
//    pthread_mutex_t lock;
//#endif
};

void hashmap_init(struct hashmap_t *map);

void hashmap_free(struct hashmap_t *map);

void hashmap_put(struct hashmap_t *map, void *key, u32 key_size, void *value, u32 val_size, bool alloc_flag);
#define hashmap_sput(map, key, value, val_size, alloc_flag) \
    hashmap_put(map, key, (strlen(key) + 1) * sizeof(char), value, val_size, alloc_flag)
#define hashmap_ssput(map, key, value, alloc_flag) \
    hashmap_put(map, key, (strlen(key) + 1) * sizeof(char), value, (strlen(value) + 1) * sizeof(char), alloc_flag)

void *hashmap_get(struct hashmap_t *map, void *key, u32 key_size);
#define hashmap_sget(map, key) \
    hashmap_get(map, key, (strlen(key) + 1) * sizeof(char))


bool hashmap_rm(struct hashmap_t *map, void *key, u32 key_size);
#define hashmap_srm(map, key) \
    hashmap_rm(map, key, (strlen(key) + 1) * sizeof(char))

/* darr */
struct darr_t {
    void **data;
    size_t size;
    size_t cap;
};

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
bool darr_rm(struct darr_t *da, size_t idx);

/*
 * removes the given value parameter from the list if it is found
 * NOTE: you can only use this function if all the elements no less than the given size parameter
 */
bool darr_rmv(struct darr_t *da, void *val, size_t size);
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

bool ht_rm(struct ht_t *ht, void *key, size_t key_size);
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

/* return codes for insert() function */
#define _HM_FULL 1
#define _HM_OVERRIDE 2
#define _HM_SUCCESS 3

static u32 hash_func_m(void *data, u32 size)
{
    /* gigahafting kok (legger dermed ikke så mye lit til det) */
    u32 A = 1327217885;
    u32 k = 0;
    for (u32 i = 0; i < size; i++)
	k += (k << 5) + ((u8 *)data)[i];

    return k * A;
}

static inline u8 hm_hash_extra(u32 hash)
{
    return (u8)(hash & ((1 << 8) - 1));
}

static inline void entry_free(struct hm_entry_t *entry)
{
    free(entry->key);
    if (entry->alloc_flag)
        free(entry->value);
}

static inline void insert_entry(struct hm_entry_t *found, struct hm_entry_t *new)
{
    //TODO: should we just set the value of found to be the value of new?

    if (found->key != NULL)
        free(found->key);
    found->key = malloc(new->key_size);
    memcpy(found->key, new->key, new->key_size);

    /* 
     * if already alloced space is sufficient, use that
     * if space is not sufficient, realloc
     */
    if (!new->alloc_flag) {
        if (found->alloc_flag)
            free(found->value);
        found->value = new->value;
    } else {
        if (!found->alloc_flag)
            found->value = malloc(new->value_size);
        else if (new->value_size > found->value_size)
            found->value = realloc(found->value, new->value_size);
        memcpy(found->value, new->value, new->value_size);
    }

    found->key_size = new->key_size;
    found->value_size = new->value_size;
    found->hash_extra = new->hash_extra;
    found->alloc_flag = new->alloc_flag;
}

static int insert(struct hm_bucket_t *bucket, struct hm_entry_t *new)
{
    /*
     * Our hashmap implementation does not allow duplcate keys.
     * Therefore, we cannot simply find the first empty entry and set the new entry here, we have 
     * to make sure the key we are inserting does not already exist somewhere else in the bucket.
     * If we find a matching key, we simply override. If we do not find a matching entry, we
     * insert the new entry in the first found empty entry.
     */
    struct hm_entry_t *found = NULL;
    bool override = false;
    for (u8 i = 0; i < HM_BUCKET_SIZE; i++) {
        //TODO må jeg ta peker?
        struct hm_entry_t *entry = &bucket->entries[i];
        if (entry->key != NULL && new->hash_extra == entry->hash_extra && new->key_size == entry->key_size) {
            if (memcmp(new->key, entry->key, new->key_size) == 0) {
                found = entry;
                override = true;
                break;
            }
        }

        if (found == NULL && entry->key == NULL)
            found = entry;
    }

    if (found == NULL)
        return _HM_FULL;

    insert_entry(found, new);
    return override ? _HM_OVERRIDE : _HM_SUCCESS;
}

static struct hm_entry_t *get_from_bucket(struct hm_bucket_t *bucket, void *key, u32 key_size, u8 hash_extra)
{
    struct hm_entry_t entry;
    for (u8 i = 0; i < HM_BUCKET_SIZE; i++) {
        entry = bucket->entries[i];
        if (key_size == entry.key_size && hash_extra == entry.hash_extra) {
            //TODO: instead of memcmp, use a function that exits if a byte is not equal
            if (memcmp(key, entry.key, key_size) == 0)
                return &bucket->entries[i];
        }
    }
    return NULL;
}

static struct hm_entry_t *get_entry(struct hashmap_t *map, void *key, u32 key_size)
{
    if (map->len == 0)
        return NULL;

    u32 hash = hash_func_m(key, key_size);
    u32 idx = hash >> (32 - map->size_log2);
    u8 extra = hm_hash_extra(hash);
    struct hm_bucket_t *bucket = &map->buckets[idx];
    return get_from_bucket(bucket, key, key_size, extra);
}

void *hashmap_get(struct hashmap_t *map, void *key, u32 key_size)
{
    struct hm_entry_t *entry = get_entry(map, key, key_size);
    if (entry == NULL)
        return NULL;
    return entry->value;
}

static void re_insert(u32 size_log2, struct hm_bucket_t *buckets, struct hm_entry_t *entry)
{

    u32 hash = hash_func_m(entry->key, entry->key_size);
    u32 idx = hash >> (32 - size_log2);
    insert(&buckets[idx], entry);
}

static void increase(struct hashmap_t *map)
{
    // TODO: instead of moving all entries in one go, we can only move over some buckets
    //       and invalidate the rest and move them once necesary

    map->size_log2++;
    assert(map->size_log2 < 32);

    int n_buckets = N_BUCKETS(map->size_log2);
    struct hm_bucket_t *new_buckets = malloc(sizeof(struct hm_bucket_t) * n_buckets);
    for (int i = 0; i < n_buckets; i++) {
        //NOTE: trenger det å være en peker?
        struct hm_bucket_t *bucket = &new_buckets[i];
        /* set all entries to NULL */
        for (u8 j = 0; j < HM_BUCKET_SIZE; j++) {
            bucket->entries[j].key = NULL;
            bucket->entries[j].alloc_flag = false;
        }
    }

    /* move all entries into the new buckets */
    int old_n_buckets = N_BUCKETS(map->size_log2 - 1);
    for (int i = 0; i < old_n_buckets; i++) {
        struct hm_bucket_t bucket = map->buckets[i];
        for (u8 j = 0; j < HM_BUCKET_SIZE; j++) {
            if (bucket.entries[j].key != NULL) {
                re_insert(map->size_log2, new_buckets, &bucket.entries[j]);
                entry_free(&bucket.entries[j]);
            }
        }
    }

    free(map->buckets);
    map->buckets = new_buckets;
}

void hashmap_put(struct hashmap_t *map, void *key, u32 key_size, void *value, u32 val_size, bool alloc_flag)
{
    double load_factor = (double)map->len / (N_BUCKETS(map->size_log2) * HM_BUCKET_SIZE);

    if (load_factor >= 0.75)
        increase(map);

    u32 hash = hash_func_m(key, key_size);
    u32 idx = hash >> (32 - map->size_log2);
    u8 extra = hm_hash_extra(hash);
    struct hm_entry_t new = { key, value, key_size, val_size, extra, alloc_flag };
    int rc = insert(&map->buckets[idx], &new);

    if (rc == _HM_FULL) {
        increase(map);
        hashmap_put(map, key, key_size, value, val_size, alloc_flag);
    }

    if (rc == _HM_SUCCESS)
        map->len++;
}


void hashmap_init(struct hashmap_t *map)
{
    map->len = 0;
    map->size_log2 = HM_STARTING_BUCKETS_LOG2;

    int n_buckets = N_BUCKETS(map->size_log2);
    map->buckets = malloc(sizeof(struct hm_bucket_t) * n_buckets);
    for (int i = 0; i < n_buckets; i++) {
        struct hm_bucket_t *bucket = &map->buckets[i];
        /* set all entries to NULL */
        for (u8 j = 0; j < HM_BUCKET_SIZE; j++) {
            bucket->entries[j].key = NULL;
            bucket->entries[j].alloc_flag = false;
        }
    }
}

bool hashmap_rm(struct hashmap_t *map, void *key, u32 key_size)
{
    struct hm_entry_t *entry = get_entry(map, key, key_size);
    if (entry == NULL)// || entry->value == NULL)
        return false;

    free(entry->key);
    entry->key = NULL;
    if (entry->alloc_flag) {
        free(entry->value);
        entry->alloc_flag = false;
    }

    map->len--;
    return true;
}

void hashmap_free(struct hashmap_t *map)
{
    int n_buckets = N_BUCKETS(map->size_log2);
    for (int i = 0; i < n_buckets; i++) {
        struct hm_bucket_t *bucket = &map->buckets[i];
        /* set all entries to NULL */
        for (u8 j = 0; j < HM_BUCKET_SIZE; j++) {
            struct hm_entry_t *entry = &bucket->entries[j];
            if (entry->key != NULL)
                entry_free(entry);
        }
    }

    free(map->buckets);
}

#endif /* NICC_HASHMAP_IMPLEMENTATION */


#ifdef NICC_DARR_IMPLEMENTATION

/* darr implementation */
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

bool darr_rm(struct darr_t *da, size_t idx)
{
    if (idx > da->size)
        return false;

    for (size_t i = idx + 1; i < da->size; i++)
        da->data[i - 1] = da->data[i];

    da->size--;
    return true;
}

bool darr_rmv(struct darr_t *da, void *val, size_t size)
{
    for (size_t i = 0; i < da->size; i++) {
        if (memcmp(da->data[i], val, size) == 0) {
            return darr_rm(da, i);
        }
    }
    return false;
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

bool ht_rm(struct ht_t *ht, void *key, size_t key_size)
{
    if (ht->len == 0)
        return false;

    size_t hash_f = hash_func(key, key_size);
    size_t hash = hash_f % ht->bucket_capacity;
    uint8_t extra = hash_extra(hash);

    struct ht_item_t *item = ht->buckets[hash];

    if (item == NULL)
        return false;

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
            return true;
        }

    cont:
        /* walk to next */
        prev = item;
        item = prev->next;
        i++;
    }

    return false;
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
