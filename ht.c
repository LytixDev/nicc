#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "common.h"

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
            free(item->value);
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
    size_t hash = hash_func(key, key_size) % ht->bucket_capacity;
    struct ht_item_t *found;
    struct ht_item_t *item = ht->buckets[hash];

    /* no ht_item means hash empty, insert immediately */
    if (item == NULL) {
        found = ht_item_add(key, key_size, value, (uint8_t)(hash >> (sizeof(size_t) - 8)));
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
    prev->next = ht_item_add(key, key_size, value, (uint8_t)(hash >> (sizeof(size_t) - 8)));
    ht->len++;
}

void *ht_get(struct ht_t *ht, void *key, size_t key_size)
{
    size_t hash = hash_func(key, key_size) % ht->bucket_capacity;
    struct ht_item_t *item = ht->buckets[hash];

    while (item != NULL) {
        if (item->hash_extra == (uint8_t)(hash >> (sizeof(size_t) - 8)))
            if (memcmp(key, item->key, key_size) == 0)
                return item->value;

        item = item->next;
    }

    return NULL;
}

void ht_rm(struct ht_t *ht, void *key, size_t key_size)
{
    size_t hash = hash_func(key, key_size) % ht->bucket_capacity;
    struct ht_item_t *item = ht->buckets[hash];

    if (item == NULL)
        return;

    struct ht_item_t *prev;
    int i = 0;

    while (item != NULL) {
        if (item->hash_extra != (uint8_t)(hash >> (sizeof(size_t) - 8)))
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
