# nicc
Generic types for C11.

### Datastructures
- [x] dynamic hashtable (hashmap_t / HashMap)*
- [x] dynamic array (arraylist_t / ArrayList)
- [x] heap queue (heapq_t)
- [ ] circular queue
- [x] stack (stack_t)*

\* hashmap implementation mirrors https://github.com/DHPS-Solutions/dhps-lib/blob/main/hashmap.c <br>
\* stack implementation is based on https://github.com/DHPS-Solutions/dhps-lib/blob/main/stack.c <br>

### Setup
The easiest is to head over to the `stb` branch, clone or copy the `nicc.h` file and include the header file into your C project:
```c
#define NICC_IMPLEMENTATION
#include "nicc.h"
```
This gives you access to all datatypes provided by the project. If you only want a certain datatype, for example hashtable, you could do:
```c
#define NICC_HASHMAP_IMPLEMENTATION
#include "nicc.h"
```

See `examples/` for examples on how to use the interface provided.
