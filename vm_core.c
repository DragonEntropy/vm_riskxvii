#include "vm_core.h"
#include "heapbank.h"
#include "utilities.h"

// Dumps the register values and program counter
void register_dump(uint32_t* registers, int PC) {
	char output[MAX_HEX_SIZE];
	int_to_hex(output, PC, 8, 1);
	printf("PC = %s;", output);
	for (int i = 0; i < REGISTERS_SIZE; i++) {
		int_to_hex(output, registers[i], 8, 1);
		printf("\nR[%d] = %s;", i, output);
	}
	printf("\n");
}

/* Dumps the memory values and program counter (purely for debug)
void memory_dump(Memory* memory, int PC) {
	char output[MAX_HEX_SIZE];
	int_to_hex(output, PC, 8, 1);
	printf("PC = %s;", output);
	for (int i = 0; i < DATA_MEMORY_SIZE; i++) {
		int_to_hex(output, memory->data_memory[i], 8, 1);
		printf("\nM[%d] = %s;", i, output);
	}
	printf("\n");
}
*/

// Ends the programs and dumps memory when called
void not_implemented_error(uint32_t instruction, uint32_t* registers, int PC) {
	char output[MAX_HEX_SIZE];
	int_to_hex(output, instruction, 2 * INSTRUCTION_SIZE, 1);
	printf("Instruction Not Implemented: %s\n", output);
	register_dump(registers, PC);
	exit(1);
}

// Ends the program and dumps memory when called
void illegal_operation_error(uint32_t instruction, uint32_t* registers, int PC) {
	char output[MAX_HEX_SIZE];
	int_to_hex(output, instruction, 2 * INSTRUCTION_SIZE, 1);
	printf("Illegal Operation: %s\n", output);
	register_dump(registers, PC);
	exit(1);
}

// Accesses memory from instruction / data memory
uint32_t* access_memory(Memory* memory, int index, int is_write) {
	// Checks if memory is data memory
	if (index >= INSTRUCTION_SIZE * INSTRUCTION_MEMORY_SIZE && index < INSTRUCTION_SIZE * INSTRUCTION_MEMORY_SIZE + DATA_SIZE * DATA_MEMORY_SIZE) {
		index -= INSTRUCTION_SIZE * INSTRUCTION_MEMORY_SIZE;
		return (uint32_t*)((uint8_t*)memory->data_memory + index);
	}

	// Checks if memory is instruction memory
	else if (index >= 0 && index < INSTRUCTION_SIZE * INSTRUCTION_MEMORY_SIZE) {

		// Checks if data is being written to instruction memory
		if (is_write) {
			printf("Illegal Operation: Cannot write to instruction memory\n");
			exit(1);
		}

		// Otherwise reads instruction memory
		return (uint32_t*)((uint8_t*)memory->instruction_memory + index);
	}

	// Otherwise raises an error
	printf("Illegal Operation: Memory address not found\n");
	exit(1);
	return 0;
}

// Accesses memory from a heapbank
uint32_t* access_heap(Heapbank* heap, int index) {
	return (uint32_t*)((uint8_t*)heap->contents + index);
}

// Handles when data is being stored to a virtual address
void store_virtual_routine(uint32_t rs1, uint8_t rs2, Memory* M, uint32_t* R, int PC, uint8_t func3, uint32_t instruction) {
	// Checks if the virtual address is a heap
	if (rs1 >= HEAP_MEMORY_OFFSET - VIRTUAL_ROUTINE_OFFSET) {
		if (heap_sb(rs1 - (HEAP_MEMORY_OFFSET - VIRTUAL_ROUTINE_OFFSET), rs2, R, M->heap_memory, func3)) {
			illegal_operation_error(instruction, R, PC);
		}
		return;
	}

	// Otherwise determines the virtual routine
	switch (rs1) {
		char output[MAX_HEX_SIZE];

		case CONSOLE_WRITE_CHAR:
			//printf("%d", (uint8_t)R[rs2]);
			printf("%c", (uint8_t)R[rs2]);
			// print_binary(R[rs2]);
			break;

		case CONSOLE_WRITE_SIGNED_INT:
			printf("%d", R[rs2]);
			break;

		case CONSOLE_WRITE_UNSIGNED_INT:
			int_to_hex(output, R[rs2], 0, 0);
			printf("%s", output);
			break;

		case HALT:
			printf("CPU Halt Requested\n");
			exit(1);

		case DUMP_PC:
			int_to_hex(output, PC, 8, 1);
			printf("%s\n", output);
			break;

		case DUMP_REGISTER_BANKS:
			register_dump(R, PC);
			break;

		case DUMP_MEMORY_WORD:
			int_to_hex(output, *access_memory(M, rs1, 0), 8, 1);
			printf("%s\n", output);
			break;

		case MALLOC:
			memory_allocate(R[rs2], M->heap_memory, R);
			break;	

		case FREE:
			if (memory_free(R[rs2], M->heap_memory)) {
				illegal_operation_error(instruction, R, PC);
			}
			break;

		// If no virtual routine is found, an error is thrown
		default:
			illegal_operation_error(instruction, R, PC);
			break;
	}
}

// Stores a value to the heap
int heap_sb(uint32_t address, uint8_t rs2, uint32_t* R, Heapbank* H, uint8_t func3) {
	int heap_index = address / HEAP_SIZE;
	int start_offset = address % HEAP_SIZE;

	Heapbank* heap = &(H[heap_index]);

	// Checks if the heap is valid
	if (heap_index < 0 || heap_index >= HEAP_MEMORY_SIZE) {
		return 1;
	}
	else if (!heap->is_used) {
		return 2;
	}
	// printf("Heap Index: %d, Offset: %d, Before Value: %d\n", heap_index, start_offset, *access_heap(heap, start_offset));

	// sb
	if (func3 == 0) {
		*access_heap(heap, start_offset) = instruction_slice(R[rs2], 7, 0, 0);
	}

	// sh
	else if (func3 == 1) {
		*access_heap(heap, start_offset) = instruction_slice(R[rs2], 15, 0, 0);
	}

	// sw
	else if (func3 == 2) {
		*access_heap(heap, start_offset) = R[rs2];
	}

	return 0;
}

// Handles when data is being retrieved from a virtual address
void load_virtual_routine(uint32_t rs1, uint8_t rd, Memory* M, uint32_t* R, int PC, uint8_t func3, uint32_t instruction) {
	char temp_char;
	int32_t temp_int;

	// Checks if the virtual address is a heap
	if (rs1 >= HEAP_MEMORY_OFFSET - VIRTUAL_ROUTINE_OFFSET) {
		if (heap_lb(rs1 - (HEAP_MEMORY_OFFSET - VIRTUAL_ROUTINE_OFFSET), rd, R, M->heap_memory, func3)) {
			illegal_operation_error(instruction, R, PC);
		}
		return;
	}

	// Otherwise determines the virtual routine
	switch (rs1) {
		case CONSOLE_READ_CHAR:
			if (!scanf("%c", &temp_char)) {
				exit(1);
			}
			R[rd] = temp_char;
			break;

		case CONSOLE_READ_SIGNED_INT:
			if (!scanf("%"PRIi32"", &temp_int)) {
				exit(1);
			}
			R[rd] = temp_int;
			break;

		default:
			break;
	}
}

// Retrieves a value from the heap
int heap_lb(uint32_t address, uint8_t rd, uint32_t* R, Heapbank* H, uint8_t func3) {
	int heap_index = address / HEAP_SIZE;
	int start_offset = address % HEAP_SIZE;

	// Checks if the heap is valid
	Heapbank* heap = &(H[heap_index]);
	if (heap_index < 0 || heap_index >= HEAP_MEMORY_SIZE) {
		return 1;
	}
	else if (!heap->is_used) {
		return 2;
	}

	// lb
	if (func3 == 0) {
		int32_t byte = *access_heap(heap, start_offset);
		R[rd] = instruction_slice(byte, 7, 0, 1);
	}

	// lh
	else if (func3 == 1) {
		int32_t half = *access_heap(heap, start_offset);
		R[rd] = instruction_slice(half, 15, 0, 1);
	}

	// lw
	else if (func3 == 2) {
		R[rd] = *access_heap(heap, start_offset);
	}

	// lbu
	else if (func3 == 4) {
		int32_t byte = *access_heap(heap, start_offset);
		R[rd] = instruction_slice(byte, 7, 0, 0);
	}
	
	// lhu
	else if (func3 == 5) {
		int32_t half = *access_heap(heap, start_offset);
		R[rd] = instruction_slice(half, 15, 0, 0);
	}

	return 0;
}