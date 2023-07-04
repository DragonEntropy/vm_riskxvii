#include "heapbank.h"
#include "utilities.h"

// Allocates memory in the heap of sufficient size
int memory_allocate(uint32_t byte_size, Heapbank* H, uint32_t* R) {
	int size = (byte_size - 1) / HEAP_SIZE + 1;

	// Checks if the size is positive
	if (size < 1) {
		R[28] = 0;
		return 1;
	}

	// Attempts to find a contiguous sequence of free heaps
	int memory_found = -1;
	for (int start_index = 0; start_index < HEAP_MEMORY_SIZE - size + 1; start_index++) {
		for (int i = 0; i < size; i++) {
			if ((H + start_index + i)->is_used) {
				break;
			}

			if (i == size - 1) {
				memory_found = start_index;
			}
		}

		if (memory_found != -1) {
			break;
		}
	}

	// Checks if any memory has been found
	if (memory_found == -1) {
		R[28] = 0;
		return 2;
	}
	
	// Allocates the required memory
	Heapbank* heap = H + memory_found;
	heap->is_first = 1;
	for (int i = 0; i < size; i++) {
		heap->is_used = 1;
		heap->has_next = 1;
		heap++;
	}
	(H + memory_found + size - 1)->has_next = 0;

	// Stores the heapbank's memory address
	R[28] = HEAP_SIZE * memory_found + HEAP_MEMORY_OFFSET;
	return 0;
}

// Frees allocated memory
int memory_free(uint32_t address, Heapbank* H) {
	Heapbank* heap = H + (address - HEAP_MEMORY_OFFSET) / HEAP_SIZE;

	// Checks if freeing the memory is valid
	if (!heap->is_used || !heap->is_first) {
		return 1;
	}

	// Frees the required memory and resets memory contents
	heap->is_first = 0;
	while (heap->has_next) {
		heap->is_used = 0;
		heap->has_next = 0;
		memset(heap->contents, 0, HEAP_SIZE);
		heap++;
	}
	heap->is_used = 0;

	return 0;
}