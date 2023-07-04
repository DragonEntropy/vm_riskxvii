#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#define INSTRUCTION_SIZE (4) // Each instruction is 4 bytes
#define DATA_SIZE (4) // All data in memory is 4 bytes
#define BYTE_SIZE (8) // Each byte has 8 bits
#define MAX_HEX_SIZE (11) // A hexadecimal output requires a maximum of 11 characters

int find_byte_length(uint32_t num);
void int_to_hex(char* output, uint32_t decimal, int size, int is_formatted);
uint32_t instruction_slice(uint32_t instruction, int start_bit, int end_bit, int sign_extend);
uint32_t int_rotate(uint32_t data, int rotation);
void print_binary(uint32_t data);

#endif