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

#include <stdlib.h>     // malloc, free, size_t type
#include <string.h>     // memcpy
#include <stdbool.h>    // bool type
#include <stdio.h>      // printf
#include <stdint.h>     // uint8_t type

#include "doubly_linked_list.h"


struct doubly_linked_list_node_t *dll_node_malloc(void *data, size_t data_size)
{
    struct doubly_linked_list_node_t *node = malloc(sizeof(struct doubly_linked_list_node_t));
    node->data = malloc(data_size);
    memcpy(node->data, data, data_size);
    node->data_size = data_size;

    return node;
}

void dll_free(struct doubly_linked_list_node_t *head)
{
    struct doubly_linked_list_node_t *node = head, *tmp;

    while (node != NULL) {
        tmp = node->next;
        free(node->data);
        free(node);
        node = tmp;
    }
}

void dll_new_head(struct doubly_linked_list_node_t **head, void *data, size_t data_size)
{
    struct doubly_linked_list_node_t *new = dll_node_malloc(data, data_size);
    new->next = *head;
    new->previous = NULL;

    (*head)->previous = new;
    *head = new;
}

void dll_insert(struct doubly_linked_list_node_t **ref, void *data, size_t data_size)
{
    struct doubly_linked_list_node_t *new = dll_node_malloc(data, data_size);
    new->next = (*ref)->next;
    new->next->previous = new;

    (*ref)->next = new;
    new->previous = *ref;
}

void dll_append(struct doubly_linked_list_node_t **ref, void *data, size_t data_size)
{
    struct doubly_linked_list_node_t *new = dll_node_malloc(data, data_size);

    while ((*ref)->next != NULL)
        *ref = (*ref)->next;;

    (*ref)->next = new;
    new->previous = *ref;
    new->next = NULL;
}

static void dll_node_print(struct doubly_linked_list_node_t *ref, size_t position)
{
    printf("rel pos [%zu]", position);
    /* print hex representation of data */
    printf("\thex:");
    for (size_t byte = 0; byte < ref->data_size; byte++)
        printf("%02x ", ((uint8_t *)ref->data)[byte]);

    putchar('\n');
}

void dll_print(struct doubly_linked_list_node_t *ref, bool traverse_to_start)
{
    if (traverse_to_start)
        while (ref->previous != NULL)
            ref = ref->previous;

    for (size_t i = 0; ref->next != NULL; ref = ref->next, i++)
        dll_node_print(ref, i);
}
