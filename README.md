# nicc
Generic types for C11.

### Types
- [x] hashtable
- [x] dynamic array
- [x] heap queue
- [ ] stack
- [ ] variable bucket hashtable

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
