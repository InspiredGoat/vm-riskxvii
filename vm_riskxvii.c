#include <stdlib.h>
#include <stdio.h>

// using these macros to avoid typos
#define REG_COUNT 32

#define INSTRUCTION_MEMORY_SIZE 1024
#define DATA_MEMORY_SIZE 1024

#define logerror(msg) 

typedef unsigned int    u32;
typedef signed int      i32;
typedef unsigned char   byte;
typedef unsigned char   byte;

typedef struct {
} InputBlob;

void print_int_as_hex_string(u32 integer) {
    putchar('0');
    putchar('x');
    
    // every byte has 2 hex digits (hexgits?)
    for (int i = 2 * 4 - 1; i >= 0; i--) {
        // every hex digit is made up of 4 bits, so shift to current digit
        // and mask them with &
        unsigned char c = (integer >> (i * 4)) & 0xf;
        
        if (c <= 9) {
            putchar('0' + c);
        }
        else {
            putchar('a' + (c - 10));
        }
    }
}

void register_dump(u32 program_counter, const u32* registers) {
    printf("PC = "); 
    print_int_as_hex_string(program_counter);
    printf(";\n");

    for (int i = 0; i < REG_COUNT; i++) {
        printf("R[%u] = ", i);
        print_int_as_hex_string(registers[i]);
        printf(";\n");
    }
}

int main(int argc, char** argv) {

    if (argc != 1) {
        printf("Error, no input blob");
        return 1;
    }

    byte instruction_memory[INSTRUCTION_MEMORY_SIZE];
    byte data_memory[DATA_MEMORY_SIZE];

    // Scope to avoid using input_file pointer again
    {
        FILE* input_file = fopen(argv[0], "r");
        unsigned int bytes_read_length = 0;

        bytes_read_length  = fread(instruction_memory, 1, INSTRUCTION_MEMORY_SIZE, input_file);
        bytes_read_length += fread(instruction_memory, 1, DATA_MEMORY_SIZE, input_file);

        fclose(input_file);
        if (bytes_read_length != INSTRUCTION_MEMORY_SIZE + DATA_MEMORY_SIZE) {
            printf("Binary blob in invalid format, expected 2048 bytes got %i.\n", bytes_read_length);
            return 1;
        }
    }

    // stored in bytes
    u32 program_counter = 0;
    const u32 R[REG_COUNT];

    while(1) {
        u32 instruction = instruction_memory[program_counter];
        program_counter += 4;
    }

    return 0;
}
