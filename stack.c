#include <stdlib.h>     // malloc, free, realloc, size_t type

typedef struct stack_t Stack;

struct stack_t {
    void **data;
    size_t size;
    size_t capacity;
};

Stack *stack_malloc(size_t starting_capacity)
{
    Stack *s = malloc(sizeof(Stack));
    s->data = malloc(starting_capacity * sizeof(void *));
    s->size = 0;
    s->capacity = starting_capacity;
    return s;
}

void stack_increase(Stack *s)
{
    size_t new_capacity = s->capacity * 2;
    s->data = realloc(s->data, new_capacity * sizeof(void *));
    s->capacity = new_capacity;
}

void *stack_pop(Stack *s);

void stack_free(Stack *s)
{
    while (s->size != 0)
        free(stack_pop(s));

    free(s->data);
    free(s);
}

void stack_push(Stack *s, void *e)
{
    /* no more space */
    if (s->size == s->capacity)
        stack_increase(s);

    s->data[s->size++] = e;
}

void *stack_pop(Stack *s)
{
    if (s->size == 0)
        return NULL;

    return s->data[--s->size];
}
