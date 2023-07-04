#include "vm_riskxvii.h"
#include "vm_instructions.h"
#include "vm_core.h"
#include "heapbank.h"
#include "utilities.h"

int main(int argc, char** argv) {
	// Setting up memory and registers
	Memory memory = {0};
	memset(memory.instruction_memory, 0, INSTRUCTION_SIZE * DATA_MEMORY_SIZE);
	memset(memory.data_memory, 0, DATA_SIZE * DATA_MEMORY_SIZE);
	uint32_t registers[REGISTERS_SIZE] = {0};

	// Reading image file
	char* memory_filename = argv[1];
	read_image(&memory, memory_filename);

	// Running program
	run_program(&memory, registers);

	return 0;
}

// Reads an image file from a filename
void read_image(Memory* memory, char* memory_filename) {

	FILE* program = fopen(memory_filename, "rb");

	// Checks if the file has been found
	if (!program) {
		printf("File not found error\n");
		exit(1);
	}

	uint32_t buffer;
	int size = 0;
	int index = 0;

	// Reads through instruction memory
	while (index < INSTRUCTION_MEMORY_SIZE) {
		size += fread(&buffer, INSTRUCTION_SIZE, 1, program);
		memory->instruction_memory[index] = buffer;
		// print_binary(instruction_memory[index]);
		index++;
	}

	index = 0;
	// Reads through data memory
	while (index < DATA_MEMORY_SIZE) {
		size += fread(&buffer, DATA_SIZE, 1, program);
		memory->data_memory[index] = buffer;
		// print_binary(data_memory[index]);
		index++;
	}

	// Closes the program
	fclose(program);
}

// Runs the program once the image
void run_program(Memory* memory, uint32_t* registers) {
	
	int PC = 0;
	while (PC < 4 * INSTRUCTION_MEMORY_SIZE) {
		// Reads the instruction from instruction memory
		uint32_t instruction = *access_memory(memory, PC, 0);

		// Tracks if the program counter should be incremented
		int move_counter = 1;

		Opcode opcode = instruction_slice(instruction, 6, 0, 0);

		// Distinguishes between different opcodes
		switch (opcode) {
			case ADD:
				add(instruction, registers, PC);
				break;

			case ADDI:
				addi(instruction, registers, PC);
				break;

			case LUI:
				lui(instruction, registers, PC);
				break;

			case LB:
				lb(instruction, registers, memory, PC);
				break;

			case SB:
				sb(instruction, registers, memory, PC);
				break;

			case BEQ:
				beq(instruction, registers, &PC, &move_counter);
				break;

			case JAL:
				jal(instruction, registers, &PC, &move_counter);
				break;

			case JALR:
				jalr(instruction, registers, &PC, &move_counter);
				break;

			// If the opcode is not found, returns an error
			default:
				not_implemented_error(instruction, registers, PC);
		}


		// Reset zero register
		registers[0] = 0;

		// Increments PC if required
		if (move_counter) {
			PC += 4;
		}

		// register_dump(registers, PC);
		// memory_dump(memory, PC);
	}
}	