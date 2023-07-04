#include "vm_instructions.h"
#include "heapbank.h"
#include "utilities.h"

/* Descriptions of all instructions can be found in the assignment docs
 * Instructions are divided into their Opcode groups
 * Each instruction is sliced up into its respective parts
 * If an instruction is not found, an error is returned
 */

void add(uint32_t instruction, uint32_t* R, int PC) {
	uint8_t func3 = instruction_slice(instruction, 14, 12, 0);
	uint8_t func7 = instruction_slice(instruction, 31, 25, 0);
	uint8_t rs1 = instruction_slice(instruction, 19, 15, 0);
	uint8_t rs2 = instruction_slice(instruction, 24, 20, 0);
	uint8_t rd = instruction_slice(instruction, 11, 7, 0);

	// add
	if (func3 == 0 && func7 == 0) {
		R[rd] = R[rs1] + R[rs2];
	}

	// sub
	else if (func3 == 0 && func7 == 32) {
		R[rd] = R[rs1] - R[rs2];
	}

	// xor
	else if (func3 == 4 && func7 == 0) {
		R[rd] = R[rs1] ^ R[rs2];
	}

	// or
	else if (func3 == 6 && func7 == 0) {
		R[rd] = R[rs1] | R[rs2];
	}

	// and
	else if (func3 == 7 && func7 == 0) {
		R[rd] = R[rs1] & R[rs2];
	}

	// sll
	else if (func3 == 1 && func7 == 0) {
		R[rd] = R[rs1] << R[rs2];
	}

	// srl
	else if (func3 == 5 && func7 == 0) {
		R[rd] = R[rs1] >> R[rs2];
	}

	// sra
	else if (func3 == 5 && func7 == 32) {
		R[rd] = int_rotate(R[rs1], R[rs2]);
	}

	// slt
	else if (func3 == 2 && func7 == 0) {
		R[rd] = ((int32_t)R[rs1] < (int32_t)R[rs2]) ? 1 : 0;
	}

	// sltu
	else if (func3 == 3 && func7 == 0) {
		R[rd] = (R[rs1] < R[rs2]) ? 1 : 0;
	}

	else {
		not_implemented_error(instruction, R, PC);
	}
}

void addi(uint32_t instruction, uint32_t* R, int PC) {
	uint8_t func3 = instruction_slice(instruction, 14, 12, 0);
	uint8_t rs1 = instruction_slice(instruction, 19, 15, 0);
	uint8_t rd = instruction_slice(instruction, 11, 7, 0);
	uint32_t imm = instruction_slice(instruction, 31, 20, 1);

	// addi
	if (func3 == 0) {
		R[rd] = R[rs1] + imm;
	}

	// xori
	else if (func3 == 4) {
		R[rd] = R[rs1] ^ imm;
	}

	// ori
	else if (func3 == 6) {
		R[rd] = R[rs1] | imm;
	}

	// andi
	else if (func3 == 7) {
		R[rd] = R[rs1] & imm;
	}

	// slti
	else if (func3 == 2) {
		R[rd] = ((int32_t)R[rs1] < (int32_t)imm) ? 1 : 0;
	}

	// sltiu
	else if (func3 == 3) {
		R[rd] = (R[rs1] < imm) ? 1 : 0;
	}

	else {
		not_implemented_error(instruction, R, PC);
	}
}

void lui(uint32_t instruction, uint32_t* R, int PC) {
	uint8_t rd = instruction_slice(instruction, 11, 7, 0);
	uint32_t imm = instruction_slice(instruction, 31, 12, 1) << 12;
	R[rd] = imm;
}

void lb(uint32_t instruction, uint32_t* R, Memory* M, int PC) {
	uint8_t func3 = instruction_slice(instruction, 14, 12, 0);
	uint8_t rs1 = instruction_slice(instruction, 19, 15, 0);
	uint8_t rd = instruction_slice(instruction, 11, 7, 0);
	uint32_t imm = instruction_slice(instruction, 31, 20, 1);

	// virtual routine
	if (R[rs1] + imm >= VIRTUAL_ROUTINE_OFFSET) {
		load_virtual_routine(R[rs1] + imm - VIRTUAL_ROUTINE_OFFSET, rd, M, R, PC, func3, instruction);
	}

	// lb
	else if (func3 == 0) {
		int32_t byte = *access_memory(M, R[rs1] + imm, 0);
		R[rd] = instruction_slice(byte, 7, 0, 1);
	}

	// lh
	else if (func3 == 1) {
		int32_t half = *access_memory(M, R[rs1] + imm, 0);
		R[rd] = instruction_slice(half, 15, 0, 1);
	}

	// lw
	else if (func3 == 2) {
		R[rd] = *access_memory(M, R[rs1] + imm, 0);
	}

	// lbu
	else if (func3 == 4) {
		int32_t byte = *access_memory(M, R[rs1] + imm, 0);
		R[rd] = instruction_slice(byte, 7, 0, 0);
	}
	
	// lhu
	else if (func3 == 5) {
		int32_t half = *access_memory(M, R[rs1] + imm, 0);
		R[rd] = instruction_slice(half, 15, 0, 0);
	}

	else {
		not_implemented_error(instruction, R, PC);
	}
}

void sb(uint32_t instruction, uint32_t* R, Memory* M, int PC) {
	uint8_t func3 = instruction_slice(instruction, 14, 12, 0);
	uint8_t rs1 = instruction_slice(instruction, 19, 15, 0);
	uint8_t rs2 = instruction_slice(instruction, 24, 20, 0);
	uint32_t imm1 = instruction_slice(instruction, 11, 7, 0);
	uint32_t imm2 = instruction_slice(instruction, 31, 25, 1);

	uint32_t imm = imm1 + (imm2 << 5);

	// virtual routine
	if (R[rs1] + imm >= VIRTUAL_ROUTINE_OFFSET) {
		store_virtual_routine(R[rs1] + imm - VIRTUAL_ROUTINE_OFFSET, rs2, M, R, PC, func3, instruction);
	}

	// sb
	else if (func3 == 0) {
		*access_memory(M, R[rs1] + imm, 1) = instruction_slice(R[rs2], 7, 0, 0);
	}

	// sh
	else if (func3 == 1) {
		*access_memory(M, R[rs1] + imm, 1) = instruction_slice(R[rs2], 15, 0, 0);
	}

	// sw
	else if (func3 == 2) {
		*access_memory(M, R[rs1] + imm, 1) = R[rs2];
	}

	// printf("M[%d] = %d, imm = %d, R[%d] = %d\n", R[rs1] + imm, *access_memory(M, R[rs1] + imm, 1), imm, rs1, R[rs1]);

	else {
		not_implemented_error(instruction, R, PC);
	}
}

void beq(uint32_t instruction, uint32_t* R, int* PC, int* move_counter) {
	uint8_t func3 = instruction_slice(instruction, 14, 12, 0);
	uint8_t rs1 = instruction_slice(instruction, 19, 15, 0);
	uint8_t rs2 = instruction_slice(instruction, 24, 20, 0);
	uint32_t imm1 = instruction_slice(instruction, 11, 8, 0);
	uint32_t imm2 = instruction_slice(instruction, 30, 25, 0);
	uint32_t imm3 = instruction_slice(instruction, 7, 7, 0);
	uint32_t imm4 = instruction_slice(instruction, 31, 31, 1);
	uint32_t imm = (imm1 << 1) + (imm2 << 5) + (imm3 << 11) + (imm4 << 12);

	// printf("%d: %d, %d\n", func3, (int32_t)R[rs1], (int32_t)R[rs2]);

	// beq
	if (func3 == 0) {
		if (R[rs1] == R[rs2]) {
			*PC += imm;
			*move_counter = 0;
		}
	}

	// bne
	else if (func3 == 1) {
		if (R[rs1] != R[rs2]) {
			*PC += imm;
			*move_counter = 0;
		}
	}

	// blt
	else if (func3 == 4) {
		if ((int32_t)R[rs1] < (int32_t)R[rs2]) {
			*PC += imm;
			*move_counter = 0;
		}
	}

	// bltu
	else if (func3 == 6) {
		if (R[rs1] < R[rs2]) {
			*PC += imm;
			*move_counter = 0;
		}
	}

	// bge
	else if (func3 == 5) {
		if ((int32_t)R[rs1] >= (int32_t)R[rs2]) {
			*PC += imm;
			*move_counter = 0;
		}
	}

	// bgeu
	else if (func3 == 7) {
		if (R[rs1] >= R[rs2]) {
			*PC += imm;
			*move_counter = 0;
		}
	}

	else {
		not_implemented_error(instruction, R, *PC);
	}
}

void jal(uint32_t instruction, uint32_t* R, int* PC, int* move_counter) {
	uint8_t rd = instruction_slice(instruction, 11, 7, 0);
	uint32_t imm1 = instruction_slice(instruction, 30, 21, 0);
	uint32_t imm2 = instruction_slice(instruction, 20, 20, 0);
	uint32_t imm3 = instruction_slice(instruction, 19, 12, 0);
	uint32_t imm4 = instruction_slice(instruction, 30, 30, 1);
	uint32_t imm = (imm1 << 1) + (imm2 << 11) + (imm3 << 12) + (imm4 << 20); 

	// jal
	R[rd] = *PC + 4;
	*PC = *PC + imm;
	*move_counter = 0;
}

void jalr(uint32_t instruction, uint32_t* R, int* PC, int* move_counter) {
	uint8_t func3 = instruction_slice(instruction, 14, 12, 0);
	uint8_t rs1 = instruction_slice(instruction, 19, 15, 0);
	uint8_t rd = instruction_slice(instruction, 11, 7, 0);
	uint32_t imm = instruction_slice(instruction, 31, 20, 1);

	// jalr
	if (func3 == 0) {
		R[rd] = *PC + 4;
		*PC = R[rs1] + imm;
		*move_counter = 0;
	}

	else {
		not_implemented_error(instruction, R, *PC);
	}
}