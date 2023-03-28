#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "virtual_machine.h"
#include "instruction.c"
#include "memory.c"
#include "util.c"

typedef struct {
    FILE*   blob_file;
    byte    instructions[1024];
    byte    data[1024];

    FILE*   expected_out_file;
    char*   expected_out;
    u32     expected_out_used;
    u32     expected_out_allocated;
} Test;

u32 instruction_encode(Instruction i) {
    switch(i.type) {
        case TYPE_INVALID: 
            assert(0); 
            break;
        case TYPE_I:

            break;
    }
    return 0;
}

static Test current;
void test_next(const char* name) {
    // since "current" is static and global, all fields will be initialized to zero
    // so this NULL check always works
    if (current.blob_file != NULL) {
        // previous blob is finished, so write to disk
        fwrite(current.instructions, 1, 1024, current.blob_file);
        fwrite(current.data,         1, 1024, current.blob_file);
        fclose(current.blob_file);
        memset(current.instructions, 0, 1024);
        memset(current.data, 0, 1024);
    }

    if (current.expected_out_file != NULL) {
        // previous blob is finished, so write to disk
        fwrite(current.instructions, 1, 1024, current.blob_file);
        fwrite(current.data,         1, 1024, current.blob_file);
        fclose(current.blob_file);
        memset(current.instructions, 0, 1024);
        memset(current.data, 0, 1024);
    }

    current.blob_file = fopen(name, "w");
}
void test_blob_add_instruction(u32 instruction) {
}
void test_blob_add_data(byte* data, u32 count) {

}
void test_expected_add_result(char* text) {

}
void test_expected_add_pc_dump(u32 pc) {

}
void test_expected_add_reg_dump(u32 pc, u32* registers) {
    printf("PC: %x\n", pc);
    for (int i = 0; i < 32; i++) {
        printf("R%i: %x\n",i, registers[i]);
    }
}
void test_expected_add_word_dump(u32 val) {

}


int main() {
    Instruction in;

    printf("Running Assertions...\n");
    printf("total memory size %u\n", TOTAL_MEM_SIZE);

    printf("\tTesting instruction decode...\n");

    in = instruction_decode(0x02078c63);
    in = instruction_decode(0xfe0798e3);

    printf("\tpassed...\n");


    // generate test files

    // format
    //  -
    // want:
    //  - ADD 1 to reg 1
    //  - ADD 2 to reg 2
    //  - Print to screen


    return 1;
}
