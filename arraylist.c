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
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include "arraylist.h"
#include "common.h"

/* arraylist implementation */
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

size_t arraylist_index_of(struct arraylist_t *arr, void *val, equality_fn_t *eq)
{
    if (val == NULL)
	return NICC_NOT_FOUND;
    for (size_t i = 0; i < arr->size; i++) {
	if (eq(get_element(arr, i), val)) {
	    return i;
	}
    }

    return NICC_NOT_FOUND;
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

bool arraylist_rmv(struct arraylist_t *arr, void *val, equality_fn_t *eq)
{
    if (val == NULL)
	return false;

    for (size_t i = 0; i < arr->size; i++) {
	if (eq(get_element(arr, i), val)) {
	    return arraylist_rm(arr, i);
	}
    }

    return false;
}

bool arraylist_sort(struct arraylist_t *arr, compare_fn_t *cmp)
{
    if (arr->size == 0)
	return false;

    if (arr->size == 1)
	return true;

    qsort(arr->data, arr->size, arr->T_size, cmp);
    return true;
}
