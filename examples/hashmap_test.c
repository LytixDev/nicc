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
#include <assert.h>
#include <string.h>

#include "../hashmap.h"

void foo(struct hashmap_t *map)
{
    hashmap_ssput(map, "key", "value", true);
}

int main()
{
    struct hashmap_t map;
    hashmap_init(&map);

    foo(&map);

    void *v = hashmap_sget(&map, "key");

    assert(strcmp((char *)v, "value") == 0);
    hashmap_free(&map);
}
