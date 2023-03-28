#include <stdio.h>

#include "virtual_machine.h"

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
void print_int_as_hex_string_file(FILE* out, u32 integer) {
    fprintf(out, "0");
    fprintf(out, "x");
    
    // every byte has 2 hex digits (hexgits?)
    for (int i = 2 * 4 - 1; i >= 0; i--) {
        // every hex digit is made up of 4 bits, so shift to current digit
        // and mask them with &
        unsigned char c = (integer >> (i * 4)) & 0xf;
        
        if (c <= 9) {
            fprintf(out, "%c", '0' + c);
        }
        else {
            fprintf(out, "%c", 'a' + (c - 10));
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


void memzero(void* data, u32 size) {
    for (int i = 0; i < size; i++)
        ((byte*)data)[i] = 0;
}
