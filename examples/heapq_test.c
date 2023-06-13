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
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common.h"
#include "../heapq.h"

/*
 * type we want to store in the heapq, using precedence as the
 * "weight"/"cost"/"precedence"
 */
typedef struct {
    char *name;
    int precedence;
} Tuple;

static inline i32 tuple_compare(const void *a, const void *b)
{
    return ((Tuple *)a)->precedence - ((Tuple *)b)->precedence;
}

void test_heapq_push_pop(void)
{
    Tuple a = { .name = "second", .precedence = 2 };
    Tuple b = { .name = "first", .precedence = 1 };
    Tuple c = { .name = "fourth", .precedence = 4 };
    Tuple d = { .name = "third", .precedence = 3 };

    struct heapq_t queue;
    heapq_init(&queue, tuple_compare);

    heapq_push(&queue, &a);
    heapq_push(&queue, &b);
    heapq_push(&queue, &c);
    heapq_push(&queue, &d);

    Tuple *first = heapq_pop(&queue);
    Tuple *second = heapq_pop(&queue);
    Tuple *third = heapq_get(&queue, 0);
    assert(b.name == first->name);
    assert(a.name == second->name);
    assert(d.name == third->name);
    heapq_free(&queue);
}

void test_heapq_sort(void)
{
    Tuple a = { .name = "second", .precedence = 2 };
    Tuple b = { .name = "first", .precedence = 1 };
    Tuple c = { .name = "fourth", .precedence = 4 };
    Tuple d = { .name = "third", .precedence = 3 };

    Tuple arr[] = { a, b, c, d };
    heap_sort(&arr, 4, sizeof(Tuple), tuple_compare);

    assert(strncmp(arr[0].name, c.name, strlen(c.name)) == 0);
    assert(strncmp(arr[1].name, d.name, strlen(d.name)) == 0);
    assert(strncmp(arr[2].name, a.name, strlen(a.name)) == 0);
    assert(strncmp(arr[3].name, b.name, strlen(b.name)) == 0);
}

int main(void)
{
    test_heapq_push_pop();
    test_heapq_sort();
    return 0;
}
