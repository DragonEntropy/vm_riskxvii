#include "utilities.h"

// Finds the number of hexadecimal digits required to write an integer
int find_byte_length(uint32_t num) {
	int byte_length = 0;
	while (num != 0) {
		num /= 16;
		byte_length++;
	}

	return byte_length;
}

// Converts an integer to a hexadecimal
void int_to_hex(char* output, uint32_t decimal, int size, int is_formatted) {
	int offset = 0;
	output[0] = '0';

	// Adds the '0x' if required
	if (is_formatted) {
		output[0] = '0';
		output[1] = 'x';
		output[2] = '0';
		offset = 2;
	}

	// If size is 0, the hexadecimal representation should have no leading 0s
	if (size == 0) {
		size = find_byte_length(decimal);
	}

	// If the size is still 0, then the decimal is 0 which is the only permitted exception
	if (size == 0) {
		output[offset + 1] = '\0';
		return;
	}

	// Writes each hexadecimal digit to the string
	for (int byte = 0; byte < size; byte++) {

		int value = (decimal >> 4 * byte) % 16;

		if (value < 10) {
			output[size + offset - 1 - byte] = value + '0';
		}
		else {
			output[size + offset - 1 - byte] = value - 10 + 'a';
		}
	}

	// Adds the null byte to the string
	output[size + offset] = '\0';
}

// Slices a portion an unsigned 4 byte integer, performing sign extension if required
uint32_t instruction_slice(uint32_t instruction, int start_bit, int end_bit, int sign_extend) {

	// "Slices off" the leading bits
	instruction <<= 31 - start_bit;

	// Performs sign extension if necessary
	if (sign_extend && instruction & (1 << 31)) {
		instruction = ~((~instruction) >> (31 - (start_bit - end_bit)));
	}

	// Otherwise "slices off" the trailing bits
	else {
		instruction >>= 31 - (start_bit - end_bit);
	}

	return instruction;
}

// Bitwise shifts an integer, mvoing the lost bits to the end
uint32_t int_rotate(uint32_t data, int rotation) {

	// Slices the bits moving right
	uint32_t temp = instruction_slice(data, 31, rotation, 0);

	// Slices the bits wrapping around
	uint32_t rotate = instruction_slice(data, rotation - 1, 0, 0);
	rotate <<= 32 - rotation;

	// Combines the two slices
	return temp + rotate;
}

/* Prints the binary interpretation of an instruction (purely for debug)
void print_binary(uint32_t data) {
	for (int byte = INSTRUCTION_SIZE - 1; byte >= 0; byte--) {
		printf("%d ", instruction_slice(data, BYTE_SIZE * byte + 7, BYTE_SIZE * byte, 0));
	}
	printf("\n");
}
*/