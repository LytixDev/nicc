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
#include "heapq.h"

#include <string.h>

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
    while (parent_idx >= 0 && hq->cmp(hq->items[parent_idx], hq->items[idx]) > 0) {
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
	    hq->cmp(hq->items[min_idx], heapq_right_child(hq, idx)) > 0)
	    min_idx = heapq_right_child_idx(idx);

	if (hq->cmp(hq->items[min_idx], hq->items[idx]) > 0) {
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
}

void heapq_init(struct heapq_t *hq, compare_fn_t *cmp)
{
    hq->size = 0;
    hq->capacity = HEAPQ_STARTING_CAPACITY;
    hq->items = malloc(hq->capacity * sizeof(void *));
    hq->cmp = cmp;
}

static void heap_sort_internal(u8 *left, u8 *right, size_t size, compare_fn_t cmp)
{
    struct heapq_t heap;
    heapq_init(&heap, cmp);

    u8 *p = left;
    u8 elems[(right - left) / size][size];
    u32 i = 0;
    while (p <= right) {
	memcpy(*(elems + i), p, size);
	heapq_push(&heap, *(elems + i++));
	p += size;
    }

    p = right;

    while (p >= left) {
	u8 *elem = (u8 *)heapq_pop(&heap);
	BYTE_SWAP(p, elem, size);
	p -= size;
    }

    heapq_free(&heap);
}

void heap_sort(const void *base, size_t nmemb, size_t size, compare_fn_t cmp)
{
    if (!nmemb)
	return;

    u8 *base_ptr = (u8 *)base;
    u8 *left_ptr = base_ptr;
    u8 *right_ptr = base_ptr + size * (nmemb - 1);
    heap_sort_internal(left_ptr, right_ptr, size, cmp);
}
