/*
 *  Copyright (C) 2022-2023 Callum Gran
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
#ifndef STACK_H
#define STACK_H

#ifdef NICC_TYPEDEF
typedef struct stack_t Stack;
#endif /* NICC_TYPEDEF */

#include "common.h"
#include <stdbool.h>
#include <sys/types.h>

struct stack_t {
    size_t size;
    size_t capacity;
    void **items;
};

void stack_init(struct stack_t *stack, size_t capacity);
void stack_free(struct stack_t *stack);

bool stack_empty(struct stack_t *stack);
bool stack_full(struct stack_t *stack);

bool stack_push(struct stack_t *stack, void *item);
void *stack_pop(struct stack_t *stack);
void *stack_get(struct stack_t *stack);

#endif /* STACK_H */