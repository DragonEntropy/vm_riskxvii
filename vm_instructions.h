#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "vm_core.h"

void add(uint32_t instruction, uint32_t* R, int PC);
void addi(uint32_t instruction, uint32_t* R, int PC);
void lui(uint32_t instruction, uint32_t* R, int PC);
void lb(uint32_t instruction, uint32_t* R, Memory* M, int PC);
void sb(uint32_t instruction, uint32_t* R, Memory* M, int PC);
void beq(uint32_t instruction, uint32_t* R, int* PC, int* move_counter);
void jal(uint32_t instruction, uint32_t* R, int* PC, int* move_counter);
void jalr(uint32_t instruction, uint32_t* R, int* PC, int* move_counter);

#endif