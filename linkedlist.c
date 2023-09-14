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
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "linkedlist.h"
#include "common.h"


void linkedlist_init(struct linkedlist_t *ll, u32 T_size)
{
    ll->size = 0;
    ll->head = NULL;
    ll->tail = NULL;
    ll->T_size = T_size;
}

void linkedlist_free(struct linkedlist_t *ll)
{
    struct linkedlist_item_t *next = ll->head;
    while (next != NULL) {
        struct linkedlist_item_t *this = next;
        next = next->next;
        free(this);
    }
}

void linkedlist_append(struct linkedlist_t *ll, void *data)
{
    ll->size++;

    struct linkedlist_item_t *item = malloc(sizeof(struct linkedlist_item_t));
    item->data = data;
    item->prev = NULL;
    item->next = NULL;

    /* base case where ll is empty */
    if (ll->head == NULL) {
        ll->head = item;
        ll->tail = ll->head;
        return;
    }

    struct linkedlist_item_t *old_tail = ll->tail;
    item->prev = old_tail;
    old_tail->next = item;
    ll->tail = item;
}

void linkedlist_remove_item(struct linkedlist_t *ll, struct linkedlist_item_t *to_remove)
{
    struct linkedlist_item_t *prev_item = to_remove->prev;
    struct linkedlist_item_t *next_item = to_remove->next;

    if (prev_item != NULL) {
        prev_item->next = next_item;
    } else {
        /* prev is NULL -> this is the new head */
        ll->head = next_item;
    }

    if (next_item != NULL) {
        next_item->prev = prev_item;
    } else {
        /* tail is NULL -> this is the new tail */
        ll->tail = next_item;
    }

    free(to_remove);
}

bool linkedlist_remove_idx(struct linkedlist_t *ll, size_t idx)
{
    if (idx > ll->size)
        return false;

    size_t i = 0;
    for (struct linkedlist_item_t *item = ll->head; item != NULL; item = item->next) {
        if (i == idx) {
            linkedlist_remove_item(ll, item);
            ll->size--;
            return true;
        }
        i++;
    }

    /* should not be reached */
    return false;
}

bool linkedlist_remove(struct linkedlist_t *ll, void *data)
{
    for (struct linkedlist_item_t *item = ll->head; item != NULL; item = item->next) {
        if (nicc_data_eq(item->data, data, ll->T_size)) {
            linkedlist_remove_item(ll, item);
            ll->size--;
            return true;
        }
    }

    return false;
}

void linkedlist_print(struct linkedlist_t *ll)
{
    for (struct linkedlist_item_t *item = ll->head; item != NULL; item = item->next) {
        printf("%p\n", item->data);
    }
}
