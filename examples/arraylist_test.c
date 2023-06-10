/*
 *  Copyright (C) 2022 Nicolai Brand 
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

#include "../arraylist.h"


int main(void)
{
    struct arraylist_t *arr = arraylist_alloc();
    /*
     * also valid:
     * struct arraylist_t *arr = arraylist_malloc();
     */
    char *str = "hello", *str2 = "world!";
    arraylist_append(arr, str);
    arraylist_append(arr, str2);

    /* expect "hello" */
    char *result = (char *)arraylist_get(arr, 0);
    assert(strcmp(str, result) == 0);

    /* override "hello" */
    arraylist_set(arr, "hello world!", 0);
    result = (char *)arraylist_get(arr, 0);
    assert(strcmp("hello world!", result) == 0);

    /*
     * remove "hello world" at index 0
     * this will shift index 1, which is "world!" into index 0
     */
    size_t size_pre_rm = arraylist_get_size(arr);
    arraylist_rm(arr, 0);
    size_t size_post_rm = arraylist_get_size(arr);
    result = (char *)arraylist_get(arr, 0);
    assert(size_pre_rm == size_post_rm + 1);

    /* add another value */
    arraylist_append(arr, "hey");

    /* pop last item from array */
    char *item = arraylist_pop(arr);
    /* know "hey" should be the last item */
    assert(strcmp(item, "hey") == 0);
    /* pop "world!" */
    arraylist_pop(arr);

    arraylist_append(arr, "first");
    arraylist_append(arr, "second");
    arraylist_append(arr, "third");
    arraylist_rmv(arr, "first", strlen("first") + 1);
    char *r = arraylist_get(arr, 0);
    assert(strcmp(r, "second") == 0);
    
    arraylist_free(arr);
    return 0;
}
