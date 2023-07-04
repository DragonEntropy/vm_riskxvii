#ifndef CORE_H
#define CORE_H

#include "heapbank.h"

#define DATA_MEMORY_SIZE (256) // 256 variables max
#define VIRTUAL_ROUTINE_SIZE (256) // 256 routines max
#define REGISTERS_SIZE (32) // 32 registers max
#define INSTRUCTION_MEMORY_SIZE (256) // 256 instructions max

// The offsets of each virtual routine from 0xb700
typedef enum virtual_routine_enum {
	CONSOLE_WRITE_CHAR = 0,
	CONSOLE_WRITE_SIGNED_INT = 4,
	CONSOLE_WRITE_UNSIGNED_INT = 8,
	HALT = 12,
	CONSOLE_READ_CHAR = 18,
	CONSOLE_READ_SIGNED_INT = 22,
	DUMP_PC = 32,
	DUMP_REGISTER_BANKS = 36,
	DUMP_MEMORY_WORD = 40,
	MALLOC = 48,
	FREE = 52
} Virtual_Routine;

// Memory contains the instructions, data and heapbanks
typedef struct memory_struct
{
	uint32_t instruction_memory[INSTRUCTION_MEMORY_SIZE];
	uint32_t data_memory[DATA_MEMORY_SIZE];
	Heapbank heap_memory[HEAP_MEMORY_SIZE];
} Memory;

void register_dump(uint32_t* registers, int PC);
void memory_dump(Memory* memory, int PC);
void not_implemented_error(uint32_t instruction, uint32_t* registers, int PC);
void illegal_operation_error(uint32_t instruction, uint32_t* registers, int PC);
uint32_t* access_memory(Memory* memory, int index, int is_write);
uint32_t* access_heap(Heapbank* heap, int index);
void store_virtual_routine(uint32_t rd, uint8_t rs2, Memory* M, uint32_t* R, int PC, uint8_t func3, uint32_t instruction);
void load_virtual_routine(uint32_t rs1, uint8_t rd, Memory* M, uint32_t* R, int PC, uint8_t func3, uint32_t instruction);
int heap_lb(uint32_t address, uint8_t rd, uint32_t* R, Heapbank* H, uint8_t func3);
int heap_sb(uint32_t address, uint8_t rs2, uint32_t* R, Heapbank* H, uint8_t func3);

#endif