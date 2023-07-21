# nicc
Generic and useful data structures targetting C11. Licensed under GPLv3, see `LICENSE`.

### Datastructures
- [x] dynamic hashtable (hashmap_t / HashMap)*
- [x] dynamic array (arraylist_t / ArrayList)
- [x] doubly linked list (linkedlist_t / LinkedList)
- [x] heap queue (heapq_t)
- [x] stack (stack_t)**
- [ ] circular queue

\* hashmap implementation mirrors https://github.com/DHPS-Solutions/dhps-lib/blob/main/hashmap.c <br>
\** stack implementation is based on https://github.com/DHPS-Solutions/dhps-lib/blob/main/stack.c <br>

### Setup
The easiest is to head over to the `stb` branch, clone or copy the `nicc.h` file, and include the single translation unit into your C project:
```c
#define NICC_IMPLEMENTATION
#include "nicc.h"
```
This gives you access to all datatypes provided by the project. If you only want a certain datatype, for example hashtable, you could do:
```c
#define NICC_HASHMAP_IMPLEMENTATION
#include "nicc.h"
```

### Examples
See `examples/` for examples on how to use the interface provided.

Checkout the [slash project](https://github.com/lytixdev/slash) if you want to see nicc used in a more real world scenario.
