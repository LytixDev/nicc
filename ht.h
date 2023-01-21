#ifndef HT_H
#define HT_H
#include <stdlib.h>


struct ht_t *ht_alloc(size_t capacity);

void ht_free(struct ht_t *ht);

void ht_set(struct ht_t *ht, void *key, size_t key_size, void *value);
#define ht_sset(a, b, c) ht_set((a), (b), strlen(b), (c))

void *ht_get(struct ht_t *ht, void *key, size_t key_size);
#define ht_sget(a, b) ht_get(a, b, strlen(b))

void ht_rm(struct ht_t *ht, void *key, size_t key_size);
#define ht_srm(a, b) ht_rm(a, b, strlen(b))

void **ht_raw(struct ht_t *ht);


#endif /* HT_H */
