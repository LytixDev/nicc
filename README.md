# nicc
Generic types for C11.

### Datastructures
- [x] fixed-size hashtable (ht)
- [x] dynamic hashtable (hashmap)*
- [x] dynamic array (darr)
- [x] heap queue (hq)
- [ ] circular queue
- [ ] stack

\* hashmap implementation mirrors https://github.com/DHPS-Solutions/dhps-lib/blob/main/hashmap.c

### Setup
Include the header file into your C project:
```c
#define NICC_IMPLEMENTATION
#include "nicc.h"
```
This gives you access to all datatypes provided by the project. If you only want a certain datatype, for example hashtable, you could do:
```c
#define NICC_HT_IMPLEMENTATION
#include "nicc.h"
```

See `examples/` for examples on how to use the interface provided.
