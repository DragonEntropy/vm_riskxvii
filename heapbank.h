#ifndef HEAPBANK_H
#define HEAPBANK_H

#include "utilities.h"

#define HEAP_SIZE (64) // Heaps have size 64 bytes
#define HEAP_ARRAY_SIZE (16) // 16 uint32_t have size 64 bytes per heap bank
#define HEAP_MEMORY_SIZE (128) // 128 heap banks max
#define HEAP_MEMORY_OFFSET (46848) // Heap bank offset of 0xb700
#define VIRTUAL_ROUTINE_OFFSET (2048) // Virtual routine offset of 0x0800

// Stores the contents of each heapbank
typedef struct heapbank_struct {
	uint32_t contents[HEAP_ARRAY_SIZE];
	int is_used; // Is the heapbank being used?
	int is_first; // Is the heapbank the first in the allocation?
	int has_next; // Does the allocation continue to the next heapbank?
} Heapbank;

int memory_allocate(uint32_t byte_size, Heapbank* H, uint32_t* R);
int memory_free(uint32_t address, Heapbank* H);

#endif