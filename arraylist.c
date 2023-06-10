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
#include <string.h>

#include "common.h"
#include "arraylist.h"

/* arraylist implementation */
struct arraylist_t *arraylist_alloc(void)
{
    struct arraylist_t *da = (struct arraylist_t *)malloc(sizeof(struct arraylist_t));
    arraylist_init(da);
    return da;
}

void arraylist_init(struct arraylist_t *da)
{
#ifdef DARR_STARTING_CAP
    da->cap = DARR_STARTING_CAP;
#else
    da->cap = 0;
    da->cap = GROW_CAPACITY(da->cap);
#endif

    da->size = 0;
    da->data = (void **)malloc(sizeof(void *) * da->cap);
}


void arraylist_free(struct arraylist_t *da)
{
    free(da->data);
    free(da);
}

void arraylist_set(struct arraylist_t *da, void *val, size_t idx)
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

inline void arraylist_append(struct arraylist_t *da, void *val)
{
    arraylist_set(da, val, da->size);
}

void *arraylist_get(struct arraylist_t *da, size_t idx)
{
    if (idx >= da->size)
        return NULL;

    return da->data[idx];
}

bool arraylist_rm(struct arraylist_t *da, size_t idx)
{
    if (idx > da->size)
        return false;

    for (size_t i = idx + 1; i < da->size; i++)
        da->data[i - 1] = da->data[i];

    da->size--;
    return true;
}

bool arraylist_rmv(struct arraylist_t *da, void *val, size_t size)
{
    for (size_t i = 0; i < da->size; i++) {
        if (memcmp(da->data[i], val, size) == 0) {
            return arraylist_rm(da, i);
        }
    }
    return false;
}

void *arraylist_pop(struct arraylist_t *da)
{
    void *item = da->data[da->size - 1];
    arraylist_rm(da, da->size - 1);
    return item;
}

inline size_t arraylist_get_size(struct arraylist_t *da)
{
    return da->size;
}

void **arraylist_raw_ret(struct arraylist_t *da)
{
    return da->data;
}
