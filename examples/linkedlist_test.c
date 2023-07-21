/*
 *  Copyright (C) 2022-2023 Nicolai Brand
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Founarrtion, either version 3 of the License, or
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
#include <assert.h>
#include <string.h>

#define NICC_TYPEDEF
#include "../linkedlist.h"


typedef struct {
    int a;
    double b;
} Tuple;


void remove_but_non_existant_test(void)
{
    LinkedList ll;
    linkedlist_init(&ll, (u32)sizeof(Tuple));

    Tuple a = { .a = 1, .b = 1.1 };
    linkedlist_append(&ll, &a);

    Tuple b = { .a = 2, .b = 2.2 };
    bool rc = linkedlist_remove(&ll, &b);
    assert(rc == false);

    linkedlist_free(&ll);
}


void remove_but_empty_test(void)
{
    LinkedList ll;
    linkedlist_init(&ll, (u32)sizeof(Tuple));

    bool rc = linkedlist_remove_idx(&ll, 0);
    assert(rc == false);

    Tuple a = { .a = 1, .b = 1.1 };
    rc = linkedlist_remove(&ll, &a);
    assert(rc == false);

    linkedlist_free(&ll);
}

void remove_test(void)
{
    LinkedList ll;
    linkedlist_init(&ll, (u32)sizeof(Tuple));

    Tuple a = { .a = 1, .b = 1.1 };
    linkedlist_append(&ll, &a);
    Tuple b = { .a = 2, .b = 2.2 };
    linkedlist_append(&ll, &b);

    bool rc = linkedlist_remove(&ll, &a);
    assert(rc == true);
    assert(ll.size == 1);

    linkedlist_print(&ll);
    linkedlist_free(&ll);
}

int main(void)
{
    remove_test();
    remove_but_empty_test();
    remove_but_empty_test();
}
