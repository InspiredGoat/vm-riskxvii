#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "virtual_machine.h"
#include "memory.c"
#include "util.c"
#include "instruction.c"

typedef struct {
    FILE*   blob_file;
    byte    instructions[1024];
    byte    data[1024];
    u16 d_off;
    u16 i_off;

    FILE*   expected_out_file;
    const char* folder;
} Test;

static Test g_current;
void tests_begin(const char* folder) {
    g_current.folder = folder;
}
void tests_done() {
    if (g_current.blob_file != NULL) {
        // previous blob is finished, so write to disk
        fwrite(g_current.instructions, 1, 1024, g_current.blob_file);
        fwrite(g_current.data,         1, 1024, g_current.blob_file);
        fclose(g_current.blob_file);
        memset(g_current.instructions, 0, 1024);
        memset(g_current.data, 0, 1024);
        g_current.d_off = 0;
        g_current.i_off = 0;
        g_current.blob_file = NULL;
        g_current.expected_out_file = NULL;
    }
}
void test_next(const char* name, const char* expected_name) {
    // since "current" is static and global, all fields will be initialized to zero
    // so this NULL check always works
    if (g_current.blob_file != NULL) {
        // previous blob is finished, so write to disk
        fwrite(g_current.instructions, 1, 1024, g_current.blob_file);
        fwrite(g_current.data,         1, 1024, g_current.blob_file);
        fclose(g_current.blob_file);
        memset(g_current.instructions, 0, 1024);
        memset(g_current.data, 0, 1024);
        g_current.d_off = 0;
        g_current.i_off = 0;
    }

    if (g_current.expected_out_file != NULL) {
        // previous blob is finished, so write to disk
        g_current.expected_out_file = NULL;
    }

    char buf[300];

    memzero(buf, 300);
    memcpy(buf, g_current.folder, strlen(g_current.folder));
    memcpy(buf + strlen(g_current.folder), name, strlen(name));
    g_current.blob_file         = fopen(buf, "w");

    memzero(buf, 300);
    memcpy(buf, g_current.folder, strlen(g_current.folder));
    memcpy(buf + strlen(g_current.folder), expected_name, strlen(expected_name));
    g_current.expected_out_file = fopen(buf, "w");
}
void test_blob_add_instruction(u32 instruction) {
    assert(g_current.i_off * 4 + 4 < 1024);
    g_current.instructions[g_current.i_off] = instruction;
    g_current.i_off += 1;
}
void test_blob_add_data(byte* data, u32 count) {
    assert(g_current.d_off + count < 1024);
    memcpy(g_current.data + g_current.d_off, data, count);
    g_current.d_off += count;
}
void test_expected_string(char* text) {
    fwrite(text, 1, strlen(text), g_current.expected_out_file);
}
void test_expected_cpu_halt() {
    fprintf(g_current.expected_out_file, "CPU HALT REQUESTED\n");
}
void test_expected_reg_dump(u32 pc, u32* registers) {
    fprintf(g_current.expected_out_file, "PC = "); 
    print_int_as_hex_string_file(g_current.expected_out_file, pc);
    fprintf(g_current.expected_out_file, ";\n");

    for (int i = 0; i < REG_COUNT; i++) {
        fprintf(g_current.expected_out_file, "R[%u] = ", i);
        print_int_as_hex_string_file(g_current.expected_out_file, registers[i]);
        fprintf(g_current.expected_out_file, ";\n");
    }
}
void test_expected_hex(u32 val) {
    print_int_as_hex_string_file(g_current.expected_out_file, val);
}
void test_expected_newline() {
    fprintf(g_current.expected_out_file, "\n");
}
void test_expected_word_dump(u32 val) {
}


#define t_assert(expression) _t_assert_full(expression, #expression, __LINE__)

u32 g_pass_count = 0;
u32 g_a_total = 0;
void _t_assert_full(byte expression, const char* string, u32 line_number) {
    if (expression != 0) {
        printf("\tpassed\n");
        g_pass_count++;
    }
    else {
        printf("\tAssertion: %s failed at gentests.c, line: %u\n", string, line_number);
    }
    g_a_total++;
}


int main() {
    Instruction in;

    printf("Running Assertions...\n");
    t_assert(1 + 1 == 9);


    // generate test files
    Test test;
    u32 R[32];
    memzero(R, sizeof(u32) * 32);

    tests_begin("tests/");

    // first test hurray!
    test_next("invalid.mi.in", "invalid.mi.out");
    test_blob_add_instruction(0x0);
    test_expected_string("Instruction Not Implemented: ");
    test_expected_hex(0x0);
    test_expected_newline();
    test_expected_reg_dump(0, R);

    tests_done();

    // format
    //  -
    // want:
    //  - ADD 1 to reg 1
    //  - ADD 2 to reg 2
    //  - Print to screen


    return 0;
}
