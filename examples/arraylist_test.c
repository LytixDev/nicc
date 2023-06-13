/*
 *  Copyright (C) 2022-2023 Nicolai Brand, Callum Gran
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
#include "../arraylist.h"

typedef struct {
    int a;
    double b;
} Tuple;

static inline i32 tuple_int_cmp(const void *a, const void *b)
{
    const Tuple *t1 = a;
    const Tuple *t2 = b;

    return t1->a - t2->a;
}

static inline i32 tuple_double_cmp(const void *a, const void *b)
{
    const Tuple *t1 = a;
    const Tuple *t2 = b;

    return (int)((1000) * (t1->b - t2->b));
}

static inline bool tuple_equal(const void *a, const void *b)
{
    const Tuple *t1 = a;
    const Tuple *t2 = b;

    return t1->a == t2->a && t1->b == t2->b;
}

void test_populate(ArrayList *arr)
{
    arraylist_append(arr, &(Tuple){ .a = 1, .b = 1.1 });
    arraylist_append(arr, &(Tuple){ .a = 2, .b = 2.2 });
    arraylist_append(arr, &(Tuple){ .a = 3, .b = 3.3 });
    Tuple *get = arraylist_get(arr, 0);
    assert(get->a == 1);
    assert(get->b == 1.1);

    get = arraylist_get(arr, 1);
    assert(get->a == 2);
    assert(get->b == 2.2);

    get = arraylist_get(arr, 2);
    assert(get->a == 3);
    assert(get->b == 3.3);
}

void test(void)
{
    /* ArrayList will hold values of Tuple */
    ArrayList arr;
    arraylist_init(&arr, sizeof(Tuple));

    test_populate(&arr);
    Tuple *get = arraylist_get(&arr, 0);
    assert(get->a == 1);
    assert(get->b == 1.1);

    get = arraylist_get(&arr, 1);
    assert(get->a == 2);
    assert(get->b == 2.2);

    get = arraylist_get(&arr, 2);
    assert(get->a == 3);
    assert(get->b == 3.3);

    arraylist_free(&arr);
}

void test_realloc(void)
{
    /* ArrayList will hold values of Tuple */
    ArrayList arr;
    arraylist_init(&arr, sizeof(Tuple));

    /* 9 appends will force a realloc */
    for (int i = 0; i < 9; i++)
	arraylist_append(&arr, &(Tuple){ .a = 1, .b = 1.1 });

    assert(arr.cap == 16);

    arraylist_append(&arr, &(Tuple){ .a = 10, .b = 10.1 });
    Tuple *get = arraylist_get(&arr, 9);
    assert(get->a == 10);
    assert(get->b == 10.1);

    arraylist_free(&arr);
}

void test_set(void)
{
    /* ArrayList will hold values of Tuple */
    ArrayList arr;
    arraylist_init(&arr, sizeof(Tuple));

    bool rc = arraylist_set(&arr, &(Tuple){ .a = 1, .b = 1.1 }, 0);
    assert(rc);

    Tuple *get = arraylist_get(&arr, 0);
    assert(get->a == 1);
    assert(get->b == 1.1);

    /* try to set at an idx that we can't set at */
    rc = arraylist_set(&arr, &(Tuple){ .a = 1, .b = 1.1 }, 69);
    assert(!rc);

    /* append some items */
    arraylist_append(&arr, &(Tuple){ .a = 2, .b = 2.2 });
    arraylist_append(&arr, &(Tuple){ .a = 3, .b = 3.3 });

    rc = arraylist_set(&arr, &(Tuple){ .a = 4, .b = 4.4 }, 0);
    assert(rc);

    get = arraylist_get(&arr, 0);
    assert(get->a == 4);
    assert(get->b == 4.4);

    arraylist_free(&arr);
}

void test_get_copy(void)
{
    /* ArrayList will hold values of Tuple */
    ArrayList arr;
    arraylist_init(&arr, sizeof(Tuple));

    arraylist_append(&arr, &(Tuple){ .a = 1, .b = 1.1 });

    Tuple get;
    arraylist_get_copy(&arr, 0, &get);
    assert(get.a == 1);
    assert(get.b == 1.1);

    arraylist_free(&arr);

    assert(get.a == 1);
    assert(get.b == 1.1);
}

void test_pop(void)
{
    /* ArrayList will hold values of Tuple */
    ArrayList arr;
    arraylist_init(&arr, sizeof(Tuple));

    arraylist_append(&arr, &(Tuple){ .a = 1, .b = 1.1 });

    Tuple get;
    arraylist_pop_and_copy(&arr, &get);
    assert(get.a == 1);
    assert(get.b == 1.1);

    arraylist_free(&arr);

    assert(get.a == 1);
    assert(get.b == 1.1);
}

void test_rm(void)
{
    /* ArrayList will hold values of Tuple */
    ArrayList arr;
    arraylist_init(&arr, sizeof(Tuple));

    arraylist_append(&arr, &(Tuple){ .a = 1, .b = 1.1 });
    bool rc = arraylist_rm(&arr, 0);
    assert(rc);

    Tuple *get = arraylist_get(&arr, 0);
    assert(get == NULL);

    arraylist_pop_and_copy(&arr, &get);
    assert(get == NULL);

    arraylist_append(&arr, &(Tuple){ .a = 1, .b = 1.1 });
    arraylist_append(&arr, &(Tuple){ .a = 2, .b = 2.2 });
    arraylist_append(&arr, &(Tuple){ .a = 3, .b = 3.3 });

    rc = arraylist_rmv(&arr, &(Tuple){ .a = 2, .b = 2.2 }, tuple_equal);
    assert(rc);

    arraylist_free(&arr);
}

void test_index_of(void)
{
    /* ArrayList will hold values of Tuple */
    ArrayList arr;
    arraylist_init(&arr, sizeof(Tuple));

    arraylist_append(&arr, &(Tuple){ .a = 3, .b = 1.1 });
    arraylist_append(&arr, &(Tuple){ .a = 2, .b = 2.2 });
    arraylist_append(&arr, &(Tuple){ .a = 1, .b = 3.3 });

    Tuple *get = arraylist_get(&arr, 0);
    assert(get->a == 3);

    get = arraylist_get(&arr, 1);
    assert(get->a == 2);

    get = arraylist_get(&arr, 2);
    assert(get->a == 1);

    i32 idx = arraylist_index_of(&arr, &(Tuple){ .a = 2, .b = 2.2 }, tuple_equal);

    assert(idx == 1);

    arraylist_free(&arr);
}

void test_index_of_r(void)
{
    /* ArrayList will hold values of Tuple */
    ArrayList arr;
    arraylist_init(&arr, sizeof(Tuple));

    arraylist_append(&arr, &(Tuple){ .a = 3, .b = 1.1 });
    arraylist_append(&arr, &(Tuple){ .a = 2, .b = 2.2 });
    arraylist_append(&arr, &(Tuple){ .a = 1, .b = 3.3 });

    Tuple *get = arraylist_get(&arr, 0);
    assert(get->a == 3);

    get = arraylist_get(&arr, 1);
    assert(get->a == 2);

    get = arraylist_get(&arr, 2);
    assert(get->a == 1);

    i32 idx = arraylist_index_of_r(&arr, &(Tuple){ .a = 2, .b = 2.2 }, tuple_equal);

    assert(idx == 1);

    arraylist_free(&arr);
}

void test_sort(void)
{
    /* ArrayList will hold values of Tuple */
    ArrayList arr;
    arraylist_init(&arr, sizeof(Tuple));

    arraylist_append(&arr, &(Tuple){ .a = 3, .b = 1.1 });
    arraylist_append(&arr, &(Tuple){ .a = 2, .b = 2.2 });
    arraylist_append(&arr, &(Tuple){ .a = 1, .b = 3.3 });

    Tuple *get = arraylist_get(&arr, 0);
    assert(get->a == 3);

    get = arraylist_get(&arr, 1);
    assert(get->a == 2);

    get = arraylist_get(&arr, 2);
    assert(get->a == 1);

    bool rc = arraylist_sort(&arr, tuple_int_cmp);
    assert(rc);

    get = arraylist_get(&arr, 0);
    assert(get->a == 1);

    get = arraylist_get(&arr, 1);
    assert(get->a == 2);

    get = arraylist_get(&arr, 2);
    assert(get->a == 3);

    rc = arraylist_sort(&arr, tuple_double_cmp);
    assert(rc);

    get = arraylist_get(&arr, 0);
    assert(get->b == 1.1);

    get = arraylist_get(&arr, 1);
    assert(get->b == 2.2);

    get = arraylist_get(&arr, 2);
    assert(get->b == 3.3);

    arraylist_free(&arr);
}

int main(void)
{
    test();
    test_realloc();
    test_set();
    test_get_copy();
    test_pop();
    test_rm();
    test_sort();
}
