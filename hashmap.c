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
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "hashmap.h"

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
