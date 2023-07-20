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
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"

void *nicc_internal_realloc(void *ptr, size_t new_size)
{
    void *res = realloc(ptr, new_size);
    // TODO: better error handling
    if (res == NULL)
	exit(1);

    return res;
}

bool nicc_data_eq(void *a, void *b, u32 T_size)
{
    /* bytewise comparison */
    u8 A;
    u8 B;
    for (size_t i = 0; i < (size_t)T_size; i++) {
        A = ((u8 *)a)[i];
        B = ((u8 *)b)[i];
        if (A != B)
            return false;
    }
    return true;
}

