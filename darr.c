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

#include <stdlib.h>
#include <stddef.h>

#include "darr.h"


/*
typedef struct darr_item_t {
    void *value;
    size_t mem_size;
} darr_item;
*/

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
    da->cap = 2;
#endif

    da->data = (void **)malloc(sizeof(void *) * da->cap);
    da->size = 0;
    return da;
}

void darr_free(struct darr_t *da)
{
    free(da->data);
    free(da);
}

void darr_add(struct darr_t *da, void *val, size_t idx)
{
    if (da->size >= da->cap) {
        /* increase capacity */
        da->data = (void **)realloc(da->data, da->size * 2 * sizeof(void *));
        da->cap = da->size * 2;
    }

    da->data[idx] = val;
    da->size++;
}

inline void darr_append(struct darr_t *da, void *val)
{
    darr_add(da, val, da->size);
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

