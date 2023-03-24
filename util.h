#ifndef UTIL_H
#define UTIL_H

#include "types.h"
#include "instructions.h"
#include <stdio.h>

void print_int_as_hex_string(u32 integer);

void register_dump(u32 program_counter, const u32* registers);
void register_dump_text(char* buffer, u32 program_counter, const u32* registers);
void instruction_print_name(InstructionName name, FILE* file);
void instruction_print_summary(Instruction instruction, FILE* file);

#endif

