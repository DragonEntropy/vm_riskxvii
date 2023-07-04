#include "vm_instructions.h"

// The different opcodes implemented
typedef enum opcode_enum {
	ADD = 51, // add, sub, xor, or, and, sll, srl, sra, slt, sltu
	ADDI = 19, // addi, xori, ori, andi, slti, sltiu
	LUI = 55, // lui
	LB = 3, // lb, lh, lw, lbu, lhu
	SB = 35, // sb, sh, sw
	BEQ = 99, // beq, bne, blt, bltu, bge, bgeu
	JAL = 111, // jal
	JALR = 103 // jalr
} Opcode;

void read_image(Memory* memory, char* memory_filename);
void run_program(Memory* memory, uint32_t* registers);