# Nicolai Brand (lytix.dev), Callum Gran 2022-2023
# See LICENSE for license info

OBJDIR = .obj
TESTDIR = examples

DIRS := $(shell find $(TESTDIR) -type d)

SRCS := $(shell find -type f -name "*.c")
BENCHSRCS := $(shell find $(TESTDIR) -type f -name "*.c")

OBJS := $(SRCS:%.c=$(OBJDIR)/%.o)
BENCHOBJS := $(BENCHSRCS:%.c=$(OBJDIR)/%.o)

CFLAGS = -Iinclude -Wall -Wextra -Wshadow -std=c11

.PHONY: format clean tags bear $(OBJDIR)
TARGET_ARRAYLIST_TEST = arraylist_test
TARGET_HASHMAP_TEST = hashmap_test 
TARGET_HEAPQ_TEST = heapq_test

clean:
	rm -rf $(OBJDIR) $(TARGET_ARRAYLIST_TEST) $(TARGET_HASHMAP_TEST) $(TARGET_HEAPQ_TEST)

tags:
	@ctags -R

bear:
	@bear -- make

format:
	python format.py

array_list_test:
	$(CC) ./arraylist.c ./common.c ./examples/arraylist_test.c $(CFLAGS) -o $(TARGET_ARRAYLIST_TEST)

hashmap_test:
	$(CC) ./hashmap.c ./common.c ./examples/hashmap_test.c $(CFLAGS) -o $(TARGET_HASHMAP_TEST)

heapq_test:
	$(CC) ./heapq.c ./common.c ./examples/heapq_test.c $(CFLAGS) -o $(TARGET_HEAPQ_TEST)

$(OBJDIR):
	$(foreach dir, $(DIRS), $(shell mkdir -p $(OBJDIR)/$(dir)))

$(OBJDIR)/%.o: ./%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/$(TESTDIR)/%.o: $(TESTDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@