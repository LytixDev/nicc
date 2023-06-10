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

#include "../heapq.h"

/*
 * type we want to store in the heapq, using precedence as the
 * "weight"/"cost"/"precedence"
 */
struct tuple_t {
  char *name;
  int precedence;
};

bool tuple_compare(void *a, void *b) {
  return ((struct tuple_t *)a)->precedence > ((struct tuple_t *)b)->precedence;
}

int main() {
  struct tuple_t a = {.name = "second", .precedence = 2};
  struct tuple_t b = {.name = "first", .precedence = 1};
  struct tuple_t c = {.name = "fourth", .precedence = 4};
  struct tuple_t d = {.name = "third", .precedence = 3};

  struct heapq_t *queue = heapq_aalloc(tuple_compare);
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
