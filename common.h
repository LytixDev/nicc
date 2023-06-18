/*
 *  Copyright (C) 2022-2023 Nicolai Brand, Callum Gran
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
#ifndef NICC_COMMON_H
#define NICC_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef u8
#define u8 uint8_t
#endif
#ifndef u32
#define u32 uint32_t
#endif

#ifndef i32
#define i32 int32_t
#endif

#ifndef NICC_NOT_FOUND
#define NICC_NOT_FOUND SIZE_MAX
#endif

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(type, pointer, new_size) \
    (type *)nicc_internal_realloc((pointer), sizeof(type) * (new_size))

/* internal function definitions */
void *nicc_internal_realloc(void *ptr, size_t new_size);

typedef i32 compare_fn_t(const void *, const void *);

typedef bool equality_fn_t(const void *, const void *);

#define BYTE_SWAP(a, b, size)                    \
    do {                                         \
	size_t __size = (size);                  \
	register unsigned char *__a = (a);       \
	register unsigned char *__b = (b);       \
	do {                                     \
	    register unsigned char __tmp = *__a; \
	    *__a++ = *__b;                       \
	    *__b++ = __tmp;                      \
	} while (--__size > 0);                  \
    } while (0)

#endif /* NICC_COMMON_H */
