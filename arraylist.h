/*
 *  Copyright (C) 2022 Nicolai Brand 
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
#ifndef NICC_ARRAYLIST_H
#define NICC_ARRAYLIST_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef NICC_TYPEDEF
typedef struct arraylist_t ArrayList;
#endif /* NICC_TYPEDEF */
struct arraylist_t {
    void **data;
    size_t size;
    size_t cap;
};

struct arraylist_t *arraylist_alloc(void);
void arraylist_init(struct arraylist_t *da);
void arraylist_free(struct arraylist_t *da);
/*
 * appends the given value parameter to the end of the array.
 * automatically allocates more space in the array if necessary.
 */
void arraylist_append(struct arraylist_t *da, void *val);
/*
 * adds the given value parameter to the given index parameter position.
 */
void arraylist_set(struct arraylist_t *da, void *val, size_t idx);
/*
 * removes the value stored at the given index and shifts the
 * values to the right one down.
 */
bool arraylist_rm(struct arraylist_t *da, size_t idx);

/*
 * removes the given value parameter from the list if it is found
 * NOTE: you can only use this function if all the elements no less than the given size parameter
 */
bool arraylist_rmv(struct arraylist_t *da, void *val, size_t size);
/*
 * removes and returns the last value stored in the array
 */
void *arraylist_pop(struct arraylist_t *da);
void *arraylist_get(struct arraylist_t *da, size_t idx);
size_t arraylist_get_size(struct arraylist_t *da);

void **arraylist_raw_ret(struct arraylist_t *da);

#endif /* NICC_ARRAYLIST_H */