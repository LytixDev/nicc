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

#ifndef NICC_DARR_H
#define NICC_DARR_H

#include <stddef.h>

//#define DARR_STARTING_CAP 2


/* types */
typedef struct darr_t darr;

/* functions */
struct darr_t *darr_malloc(void);

void darr_free(struct darr_t *da);

void darr_append(struct darr_t *da, void *val);

void darr_add(struct darr_t *da, void *val, size_t idx);

void darr_rm(struct darr_t *da, size_t idk);

void *darr_pop(struct darr_t *da);

void *darr_get(struct darr_t *da, size_t idx);

size_t darr_get_size(struct darr_t *da);

void darr_raw(struct darr_t *da, void *raw[]);

#endif /* NICC_DARR_H */
