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

/* 
 * common 
 */
#ifndef u8
#define u8 uint8_t
#endif
#ifndef u32
#define u32 uint32_t
#endif

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(type, pointer, new_size) \
    (type *)nicc_internal_realloc((pointer), sizeof(type) * (new_size))

/* internal function definitions */
void *nicc_internal_realloc(void *ptr, size_t new_size);
/* 
 * common end
 */

/* 
 * arraylist 
 */
typedef struct arraylist_t ArrayList;

struct arraylist_t {
    void *data;
    u32 T_size;
    size_t size;
    size_t cap;
};

void arraylist_init(struct arraylist_t *arr, u32 T_size);
void arraylist_free(struct arraylist_t *arr);

bool arraylist_set(struct arraylist_t *arr, void *val, size_t idx);
bool arraylist_append(struct arraylist_t *arr, void *val);

void *arraylist_get(struct arraylist_t *arr, size_t idx);
void arraylist_get_copy(struct arraylist_t *arr, size_t idx, void *return_ptr);
void arraylist_pop_and_copy(struct arraylist_t *arr, void *return_ptr);

bool arraylist_rm(struct arraylist_t *arr, size_t idx);
bool arraylist_rmv(struct arraylist_t *arr, void *val);
/* 
 * arraylist end
 */

/* 
 * hashmap
 */
#define HM_STARTING_BUCKETS_LOG2 3 // the amount of starting buckets
#define HM_BUCKET_SIZE 6
#define HM_OVERFLOW_SIZE 4
#define N_BUCKETS(log2) (1 << (log2))

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

typedef struct hashmap_t HashMap;

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
 * hashmap end
 */

/* 
 * heapq
 */
/* compares priority of items in heapqueue. I:E: is a > b ? */
typedef bool(nicc_hq_compare_func)(void *a, void *b);

/* heap queue inspired by: https://docs.python.org/3/library/heapq.html */
struct heapq_t {
    void **items;
    int size;
    int capacity;
    nicc_hq_compare_func *cmp;
};

/* heapq functions */
struct heapq_t *heapq_alloc(nicc_hq_compare_func *cmp);
void heapq_free(struct heapq_t *hq);
void heapq_push(struct heapq_t *hq, void *item);
void *heapq_get(struct heapq_t *hq, int idx);
/*
 * returns and removes the item at the top of the heap queue.
 * note: remember to free() the popped item after use if it was malloced before
 * pushing into the heapq.
 */
void *heapq_pop(struct heapq_t *hq);
/* 
 * heapq end
 */


#endif /* NICC_NICC_H */

#ifdef NICC_IMPLEMENTATION
#       define NICC_ARRAYLIST_IMPLEMENTATION
#       define NICC_HT_IMPLEMENTATION
#       define NICC_STACK_IMPLEMENTATION
#       define NICC_HEAPQ_IMPLEMENTATION
#       define NICC_HASHMAP_IMPLEMENTATION
#endif

#if defined NICC_IMPLEMENTATION || defined NICC_ARRAYLIST_IMPLEMENTATION || \
    defined NICC_HT_IMPLEMENTATION || defined NICC_STACK_IMPLEMENTATION || \
    defined NICC_HEAPQ_IMPLEMENTATION || defined NICC_HASHMAP_IMPLEMENTATION

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
    // TODO: should we just set the value of found to be the value of new?

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
     * Therefore, we cannot simply find the first empty entry and set the new
     * entry here, we have to make sure the key we are inserting does not already
     * exist somewhere else in the bucket. If we find a matching key, we simply
     * override. If we do not find a matching entry, we insert the new entry in
     * the first found empty entry.
     */
    struct hm_entry_t *found = NULL;
    bool override = false;
    for (u8 i = 0; i < HM_BUCKET_SIZE; i++) {
	// TODO må jeg ta peker?
	struct hm_entry_t *entry = &bucket->entries[i];
	if (entry->key != NULL &&
	    new->hash_extra == entry->hash_extra &&new->key_size == entry->key_size) {
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

static struct hm_entry_t *get_from_bucket(struct hm_bucket_t *bucket, void *key, u32 key_size,
					  u8 hash_extra)
{
    struct hm_entry_t entry;
    for (u8 i = 0; i < HM_BUCKET_SIZE; i++) {
	entry = bucket->entries[i];
	if (key_size == entry.key_size && hash_extra == entry.hash_extra) {
	    // TODO: instead of memcmp, use a function that exits if a byte is not
	    // equal
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
    // TODO: instead of moving all entries in one go, we can only move over some
    // buckets
    //       and invalidate the rest and move them once necesary

    map->size_log2++;
    assert(map->size_log2 < 32);

    int n_buckets = N_BUCKETS(map->size_log2);
    struct hm_bucket_t *new_buckets = malloc(sizeof(struct hm_bucket_t) * n_buckets);
    for (int i = 0; i < n_buckets; i++) {
	// NOTE: trenger det å være en peker?
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

void hashmap_put(struct hashmap_t *map, void *key, u32 key_size, void *value, u32 val_size,
		 bool alloc_flag)
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
    if (entry == NULL) // || entry->value == NULL)
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


#ifdef NICC_ARRAYLIST_IMPLEMENTATION

void arraylist_init(struct arraylist_t *arr, u32 T_size)
{
#ifdef DARR_STARTING_CAP
    da->cap = DARR_STARTING_CAP;
#else
    arr->cap = 0;
    arr->cap = GROW_CAPACITY(arr->cap);
#endif

    arr->size = 0;
    arr->T_size = T_size;
    arr->data = malloc(T_size * arr->cap);
}

void arraylist_free(struct arraylist_t *arr)
{
    free(arr->data);
}

static void ensure_capacity(struct arraylist_t *arr, size_t idx)
{
    if (idx >= arr->cap) {
	/* increase capacity */
	arr->cap = GROW_CAPACITY(arr->cap);
	arr->data = nicc_internal_realloc(arr->data, arr->T_size * arr->cap);
    }
}

static void *get_element(struct arraylist_t *arr, size_t idx)
{
    uintptr_t result = (uintptr_t)arr->data + (uintptr_t)(idx * arr->T_size);
    return (void *)result;
}

bool arraylist_set(struct arraylist_t *arr, void *val, size_t idx)
{
    /* this is a bit conservative maybe, but it's not possible to set a value at a
     * position greater than the size as we don't have any mechanisms to keep
     * track of this. a bit map or something may be useful, idk.
     */
    if (idx > arr->size)
	return false;
    ensure_capacity(arr, idx);
    memcpy(get_element(arr, idx), val, arr->T_size);

    /* special case where we actually appended to the arraylist */
    if (idx == arr->size)
	arr->size++;

    return true;
}

bool arraylist_append(struct arraylist_t *arr, void *val)
{
    bool success = arraylist_set(arr, val, arr->size);
    if (!success)
	return false;
    return true;
}

void *arraylist_get(struct arraylist_t *arr, size_t idx)
{
    if (idx >= arr->size)
	return NULL;

    return get_element(arr, idx);
}

void arraylist_get_copy(struct arraylist_t *arr, size_t idx, void *return_ptr)
{
    void *element = arraylist_get(arr, idx);
    if (element == NULL) {
	return_ptr = NULL;
	return;
    }

    memcpy(return_ptr, element, arr->T_size);
}

void arraylist_pop_and_copy(struct arraylist_t *arr, void *return_ptr)
{
    if (arr->size == 0) {
	return_ptr = NULL;
	return;
    }

    arraylist_get_copy(arr, arr->size - 1, return_ptr);
    arraylist_rm(arr, arr->size - 1);
}

bool arraylist_rm(struct arraylist_t *arr, size_t idx)
{
    if (idx > arr->size)
	return false;

    for (size_t i = idx + 1; i < arr->size; i++)
	memcpy(get_element(arr, i - 1), get_element(arr, i), arr->T_size);

    arr->size--;
    return true;
}

static bool element_eq(void *a, void *b, size_t size)
{
    // TODO: using int here would be faster I think
    // TODO: this is broken idk why
    u8 *A = a;
    u8 *B = b;

    for (size_t i = 0; i < size; i++) {
	if (A[i] != B[i])
	    return false;
    }
    return true;
}

bool arraylist_rmv(struct arraylist_t *arr, void *val)
{
    for (size_t i = 0; i < arr->size; i++) {
	if (element_eq(get_element(arr, i), val, arr->T_size))
	    return arraylist_rm(arr, i);
    }
    return false;
}

#endif /* NICC_ARRAYLIST_IMPLEMENTATION */


#ifdef NICC_HEAPQ_IMPLEMENTATION

#define HEAPQ_STARTING_CAPACITY 32

#define heapq_left_child_idx(parent_idx) (parent_idx * 2 + 1)
#define heapq_right_child_idx(parent_idx) (parent_idx * 2 + 2)
#define heapq_parent_idx(child_idx) ((child_idx - 1) / 2)

#define heapq_has_left(idx, size) (heapq_left_child_idx(idx) < size)
#define heapq_has_right(idx, size) (heapq_right_child_idx(idx) < size

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
	if (heapq_has_right(idx, hq->size) &&
	    hq->cmp(hq->items[min_idx], heapq_right_child(hq, idx))))
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

struct heapq_t *heapq_alloc(nicc_hq_compare_func *cmp)
{
    struct heapq_t *hq = malloc(sizeof(struct heapq_t));
    hq->size = 0;
    hq->capacity = HEAPQ_STARTING_CAPACITY;
    hq->items = malloc(hq->capacity * sizeof(void *));
    hq->cmp = cmp;
    return hq;
}

#endif /* NICC_HEAPQ_IMPLEMENTATION */
