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
#ifndef NICC_LINKEDLIST_H
#define NICC_LINKEDLIST_H

#ifdef NICC_TYPEDEF
typedef struct linkedlist_t LinkedList;
typedef struct linkedlist_item_t LinkedListItem;
#endif /* NICC_TYPEDEF */

#include <stdlib.h>
#include <stdbool.h>
#include "common.h"


struct linkedlist_item_t {
    void *data; // pointer to the data. we don't manage the memory of this.
    struct linkedlist_item_t *prev; // can be NULL
    struct linkedlist_item_t *next; // can be NULL
};

/*
 * Doubly linked list implementation.
 * Assumes all data that is stored has the same length.
 */
struct linkedlist_t {
    struct linkedlist_item_t *head;
    struct linkedlist_item_t *tail;
    size_t size; // for convenience
    u32 T_size; // sizeof the value that data in linkedlist_item_t points to
};

/* functions */

/*
 * Same as `struct linkedlist_t ll = { .head = NULL, .tail = NULL, . size = 0, .T_size = T_size }`
 */
void linkedlist_init(struct linkedlist_t *ll, u32 T_size);

/*
 * Frees all the connected items in the linkedlist.
 */
void linkedlist_free(struct linkedlist_t *ll);

/*
 * Appends a new item to the end
 */
void linkedlist_append(struct linkedlist_t *ll, void *data);

/*
 * Removes the first occurence of data.
 * Uses nicc_data_eq() from common.c under the hood.
 */
bool linkedlist_remove(struct linkedlist_t *ll, void *data);
/*
 * Removes the item at given index.
 */
bool linkedlist_remove_idx(struct linkedlist_t *ll, size_t idx);

/*
 * Prints the data pointers of every item.
 */
void linkedlist_print(struct linkedlist_t *ll);

/* 
 * Simple for-each macro
 */
#define NICC_LL_FOR_EACH(ll, item) for (item = (ll)->head; item != NULL; item = item->next)


#endif /* NICC_LINKEDLIST_H */
