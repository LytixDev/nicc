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

#ifndef NICC_DOUBLY_LINKED_LIST_H
#define NICC_DOUBLY_LINKED_LIST_H

#include <stdlib.h>     // size_t type


#define DLL_PRINTABLE


/* types */

typedef struct doubly_linked_list_node_t {
    struct doubly_linked_list_node_t *next;
    struct doubly_linked_list_node_t *previous;
    void *data;
#ifdef DLL_PRINTABLE
    size_t data_size;
#endif
} dll_node;


//typedef struct doubly_linked_list_t {
//    struct doubly_linked_list_t *head;
//} dll;


/* functions */
struct doubly_linked_list_node_t *dll_node_malloc(void *data, size_t data_size);

void dll_free(struct doubly_linked_list_node_t *head);

#ifdef DLL_PRINTABLE
    void dll_print(struct doubly_linked_list_node_t *ref, bool traverse_to_start);
#endif

void dll_append(struct doubly_linked_list_node_t **ref, void *data, size_t data_size);

void dll_insert(struct doubly_linked_list_node_t **ref, void *data, size_t data_size);

void dll_new_head(struct doubly_linked_list_node_t **head, void *data, size_t data_size);



#endif /* NICC_DOUBLY_LINKED_LIST_H */
