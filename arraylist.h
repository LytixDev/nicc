/*
 *  Copyright (C) 2022-2023 Nicolai Brand, Callum Gran
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
#ifndef NICC_ARRAYLIST_H
#define NICC_ARRAYLIST_H

#include <stdbool.h>
#include <stdlib.h>

#include "common.h"

#ifdef NICC_TYPEDEF
typedef struct arraylist_t ArrayList;
#endif /* NICC_TYPEDEF */

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
bool arraylist_pop(struct arraylist_t *arr);
bool arraylist_pop_and_copy(struct arraylist_t *arr, void *return_ptr);

size_t arraylist_index_of(struct arraylist_t *arr, void *val, equality_fn_t *eq);

bool arraylist_rm(struct arraylist_t *arr, size_t idx);
bool arraylist_rmv(struct arraylist_t *arr, void *val, equality_fn_t *eq);

bool arraylist_sort(struct arraylist_t *arr, compare_fn_t *cmp);

#endif /* NICC_ARRAYLIST_H */
