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

#ifndef NICC_DYNAMIC_ARRAY_H
#define NICC_DYNAMIC_ARRAY_H

#include <stddef.h>

//#define DARR_STARTING_CAP 2


/* types */
typedef struct darr_t darr;

/* functions */
struct darr_t *darr_malloc(void);

void darr_free(struct darr_t *da);

/*
 * appends the given value parameter to the end of the array.
 * automatically allocates more space in the array if necessary.
 */
void darr_append(struct darr_t *da, void *val);

/*
 * adds the given value parameter to the given index parameter position.
 */
void darr_set(struct darr_t *da, void *val, size_t idx);


/*
 * removes the value stored at the given index and shifts the
 * values to the right one down.
 */
void darr_rm(struct darr_t *da, size_t idk);

/*
 * removes and returns the last value stored in the array
 */
void *darr_pop(struct darr_t *da);

void *darr_get(struct darr_t *da, size_t idx);

size_t darr_get_size(struct darr_t *da);

/*
 * populates the raw parameter with pointers to the values stored
 * in the array and NULL terminates it. The raw void pointer array 
 * should be of size darr_get_size() + 1.
 */
void darr_raw(struct darr_t *da, void *raw[]);

#endif /* NICC_DYNAMIC_ARRAY_H */
