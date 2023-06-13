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
#ifndef NICC_HEAPQ_H
#define NICC_HEAPQ_H

#include "common.h"
#include <stdbool.h>

#define HEAPQ_STARTING_CAPACITY 32

#define heapq_left_child_idx(parent_idx) ((parent_idx << 1) + 1)
#define heapq_right_child_idx(parent_idx) ((parent_idx + 1) << 1)
#define heapq_parent_idx(child_idx) ((child_idx - 1) >> 1)

#define heapq_has_left(idx, size) (heapq_left_child_idx(idx) < size)
#define heapq_has_right(idx, size) (heapq_right_child_idx(idx) < size)

/* heap queue inspired by: https://docs.python.org/3/library/heapq.html */
struct heapq_t {
    void **items;
    int size;
    int capacity;
    compare_fn_t *cmp;
};

/* heapq functions */
void heapq_init(struct heapq_t *hq, compare_fn_t *cmp);
void heapq_free(struct heapq_t *hq);
void heapq_push(struct heapq_t *hq, void *item);
void *heapq_get(struct heapq_t *hq, int idx);

/*
 * returns and removes the item at the top of the heap queue.
 * note: remember to free() the popped item after use if it was malloced before
 * pushing into the heapq.
 */
void *heapq_pop(struct heapq_t *hq);

void heap_sort(const void *base, size_t nmemb, size_t size, compare_fn_t *cmp);

#endif /* NICC_HEAPQ_H */
