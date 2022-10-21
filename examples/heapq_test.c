#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define NICC_HEAPQ_IMPLEMENTATION
#include "../nicc.h"

/*
 * type we want to store in the heapq, using precedence as the "weight"/"cost"/"precedence"
 */
struct tuple_t {
    char *name;
    int precedence;
};

bool tuple_compare(void *a, void *b)
{
    return ((struct tuple_t *)a)->precedence > ((struct tuple_t *)b)->precedence;
}

int main()
{
    struct tuple_t a = { .name = "second", .precedence = 2 };
    struct tuple_t b = { .name = "first", .precedence = 1 };
    struct tuple_t c = { .name = "fourth", .precedence = 4 };
    struct tuple_t d = { .name = "third", .precedence = 3 };

    struct heapq_t *queue = heapq_malloc(tuple_compare);
    heapq_push(queue, &a);
    heapq_push(queue, &b);
    heapq_push(queue, &c);
    heapq_push(queue, &d);

    struct tuple_t *first = heapq_pop(queue);
    struct tuple_t *second = heapq_pop(queue);
    struct tuple_t *third = heapq_get(queue, 0);
    assert(b.name == first->name);
    assert(a.name == second->name);
    assert(d.name == third->name);
    heapq_free(queue);
}
