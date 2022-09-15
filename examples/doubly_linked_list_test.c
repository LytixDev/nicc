/*
 * Written by Nicolai H. Brand (lytix.dev) 2022.
 * Example usage of the dynamic array implementation found in nicc.
 * Licensed under GPLv3, see the LICENSE file for more info.
 */

#include <stdio.h>
#include <stdbool.h>

#include "../doubly_linked_list.h"


int main()
{
    int val1 = 10, val2 = 11, val3 = 19393, val4 = 79;
    dll_node *head = dll_node_malloc(&val1, sizeof(val1));
    dll_append(&head, &val2, sizeof(val2));
    dll_append(&head, &val3, sizeof(val3));
    dll_append(&head, &val4, sizeof(val4));

    dll_print(head, true);

    dll_free(head);
    
    return 0;
}
