#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "util.h"
#include "instructions.h"



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
}
void test_expected_add_word_dump(u32 val) {
}


int main() {

    // generate test files

    // format
    //  -
    // want:
    //  - ADD 1 to reg 1
    //  - ADD 2 to reg 2
    //  - Print to screen


    return 1;
}
