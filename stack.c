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
#include <stdbool.h>
#include <stdlib.h>

#include "stack.h"

void stack_init(struct stack_t *stack, size_t capacity)
{
    stack->size = 0;
    stack->capacity = capacity;
    stack->items = (void **)(malloc(capacity * sizeof(void *)));
}

void stack_free(struct stack_t *stack)
{
    free(stack->items);
}

inline bool stack_empty(struct stack_t *stack)
{
    return !stack->size;
}

inline bool stack_full(struct stack_t *stack)
{
    return stack->size == stack->capacity;
}

inline bool stack_push(struct stack_t *stack, void *item)
{
    if (stack == NULL || item == NULL)
	return false;

    if (stack_full(stack))
	return false;

    stack->items[stack->size++] = item;
    return true;
}

inline void *stack_pop(struct stack_t *stack)
{
    if (stack == NULL)
	return NULL;

    if (stack_empty(stack))
	return NULL;

    return stack->items[--stack->size];
}

inline void *stack_get(struct stack_t *stack)
{
    if (stack == NULL)
	return NULL;

    if (stack_empty(stack))
	return NULL;

    return stack->items[stack->size - 1];
}