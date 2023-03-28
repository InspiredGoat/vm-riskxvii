#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "virtual_machine.h"
#include "memory.c"
#include "util.c"
#include "instruction.c"

#define DEBUG_PRINT_INSTRUCTIONS
#define DEBUG_PRINT_INSTRUCTION_FILE

typedef struct {
    byte data[64];
} MemoryBank;

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Error, no input file\n");
        return 1;
    }

    byte should_terminate = 0;
    u32 program_counter = 0;
    u32 R[REG_COUNT];

    // 3 bits per register type == 24 bytes
    byte RT[32];
    memset(R, 0, 32);
    memset(RT, 0, 32);

    byte* memory = malloc(TOTAL_MEM_SIZE);

    u32*  instruction_memory  = (u32*) &memory[0];
    byte* data_memory         = &memory[INSTRUCTION_MEMORY_SIZE];
    /* byte* dynamic_memory      = &memory[0xb700]; */
    /* MemoryBank* dynamic_banks = (MemoryBank*) &memory[0xb700]; */
    byte dynamic_banks_bit_array = 0; // stores whether or not a bank is used as a bit.

    memset(memory, 0, TOTAL_MEM_SIZE);

    // Scope to avoid using input_file pointer again

    {
        FILE* input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            printf("Failed to open file: %s\n", argv[1]);
            free(memory);
            return 1;
        }

        unsigned int bytes_read_length = 0;

        bytes_read_length  = fread(instruction_memory, 1, INSTRUCTION_MEMORY_SIZE, input_file);
        bytes_read_length += fread(data_memory, 1, DATA_MEMORY_SIZE, input_file);

        fclose(input_file);
        if (bytes_read_length != INSTRUCTION_MEMORY_SIZE + DATA_MEMORY_SIZE) {
            printf("Binary blob in invalid format, expected 2048 bytes got %i.\n", bytes_read_length);
            free(memory);
            return 1;
        }
    }

    /*           opcode        00000000000000000000000001111111; */
    /*           rd            00000000000000000000111110000000; */
    /*           rs1           00000000000011111000000000000000; */
    /*           func3         00000000000000000111000000000000; */

    /* instruction_memory[0]  = 0x7ff00113; */
    /* instruction_memory[1]  = 0x00c000ef; */
    /* instruction_memory[2]  = 0x000017b7; */
    /* instruction_memory[3]  = 0x80078623; */

    /* instruction_memory[4]  = 0x00001737; */
    /* instruction_memory[5]  = 0x81670793; */
    /* instruction_memory[6]  = 0x0007a683; */
    /* instruction_memory[7]  = 0x0007a783; */
    /* instruction_memory[8]  = 0x00d787b3; */
    /* instruction_memory[9]  = 0x80f72223; */
    /* instruction_memory[10] = 0x00000513; */
    /* instruction_memory[11] = 0x00008067; */

#ifdef DEBUG_PRINT_INSTRUCTION_FILE
    printf("PC\tCode\t\tName\tRd\tR1\tR2\tImm\n");
    Instruction in;

    do {
        // TODO: check program counter is valid
        u32 instruction_data = instruction_memory[program_counter / 4];
        VirtualInstructionName virt_instruction = VIRTUAL_NONE;
        byte failed_memory = 0;
        byte jumped = 0;

        in = instruction_decode(instruction_data);
        if (in.name == INSTRUCTION_INVALID)
            break;

        printf("%x:\t", program_counter);
        print_int_as_hex_string(instruction_data);
        putchar('\t');
        instruction_print_summary(in, stdout);
        program_counter += 4;
    }
    while (in.name != INSTRUCTION_INVALID);
    free(memory);
    exit(0);
#endif

#ifdef DEBUG_PRINT_INSTRUCTIONS
    printf("PC\tCode\t\tName\tRd\tR1\tR2\tImm\n");
#endif

        u64 executed_instructions_count = 0;
    while(program_counter < INSTRUCTION_MEMORY_SIZE) {
        Instruction in;
        // TODO: check program counter is valid
        u32 instruction_data = instruction_memory[program_counter / 4];
        VirtualInstructionName virt_instruction = VIRTUAL_NONE;
        byte failed_memory = 0;
        byte jumped = 0;

        in = instruction_decode(instruction_data);

#ifdef DEBUG_PRINT_INSTRUCTIONS
        printf("%x:\t", program_counter);
        print_int_as_hex_string(instruction_data);
        putchar('\t');
        instruction_print_summary(in, stdout);
#endif


        switch (in.name) {
            case INSTRUCTION_COUNT: break;
            case INSTRUCTION_INVALID:
                printf("Instruction Not Implemented: ");
                print_int_as_hex_string(instruction_data);
                printf("\n");
                should_terminate = 1;

                break;
            case ADD:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) + R_CAST(R[in.rs2], RT[in.rs2]);
                break;
            case ADDI:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) + in.imm;
                break;
            case SUB:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) - R_CAST(R[in.rs2], RT[in.rs2]);
                break;
            case LUI:
                R[in.rd] = (in.imm >> 12);
                break;
            case XOR:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) ^ R_CAST(R[in.rs2], RT[in.rs2]);
                break;
            case XORI:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) ^ in.imm;
                break;
            case OR:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) | R_CAST(R[in.rs2], RT[in.rs2]);
                break;
            case ORI:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) | in.imm;
                break;
            case AND:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) & R_CAST(R[in.rs2], RT[in.rs2]);
                break;
            case ANDI:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) & in.imm;
                break;
            case SLL:
                R[in.rd] = R[in.rs1] << R[in.rs2];
                break;
            case SRL:
                R[in.rd] = R[in.rs1] >> R[in.rs2];
                break;
            case SRA:
                R[in.rd] = (R[in.rs1] >> R[in.rs2]) | (R[in.rs1] << (32 - R[in.rs2]));
                break;
            case LB:
                {
                char val = 0;
                failed_memory &= get_mem_char(memory, dynamic_banks_bit_array, R[in.rs1] + in.imm, &val);
                R[in.rd] = val;
                RT[in.rd] = R_I8_FLAG;
                }
                break;
            case LH:
                {
                i16 val = 0;
                failed_memory &= get_mem_i16(memory, dynamic_banks_bit_array, R[in.rs1] + in.imm, &val);
                R[in.rd] = val;
                RT[in.rd] = R_I16_FLAG;
                }
                break;
            case LW:
                {
                i32 val = 0;

                failed_memory &= get_mem_i32(memory, dynamic_banks_bit_array, R[in.rs1] + in.imm, &val);
                R[in.rd] = val;
                RT[in.rd] = R_I32_FLAG;
                }
                break;
            case LBU:
                {
                byte val = 0;
                failed_memory &= get_mem_byte(memory, dynamic_banks_bit_array, R[in.rs1] + in.imm, &val);
                R[in.rd] = val;
                RT[in.rd] = R_U8_FLAG;
                }
                break;
            case LHU:
                {
                u16 val = 0;
                failed_memory &= get_mem_u16(memory, dynamic_banks_bit_array, R[in.rs1] + in.imm, &val);
                R[in.rd] = val;
                RT[in.rd] = R_U16_FLAG;
                }
                break;
            case SB:
                {
                failed_memory &= set_mem_byte(memory, dynamic_banks_bit_array, R[in.rs1] + in.imm, R[in.rs2], &virt_instruction);
                }
                break;
            case SH:
                failed_memory &= set_mem_u16( memory, dynamic_banks_bit_array, R[in.rs1] + in.imm, R[in.rs2], &virt_instruction);
                break;
            case SW:
                failed_memory &= set_mem_u32( memory, dynamic_banks_bit_array, R[in.rs1] + in.imm, R[in.rs2], &virt_instruction);
                break;
            case SLT:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) < R_CAST(R[in.rs2], RT[in.rs2]);
                break;
            case SLTI:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) < (i32)in.imm;
                break;
            case SLTU:
                R[in.rd] = (unsigned) R_CAST(R[in.rs1], RT[in.rs1]) < (unsigned) R_CAST(R[in.rs2], RT[in.rs2]);
                break;
            case SLTIU:
                R[in.rd] = R_CAST(R[in.rs1], RT[in.rs1]) < (i32)in.imm;
                break;
            case BEQ:
                if (R[in.rs1] == R[in.rs2]) {
                    program_counter += in.imm;
                    jumped = 1;
                }
                break;
            case BNE:
                if (R[in.rs1] != R[in.rs2]) {
                    program_counter += in.imm;
                    jumped = 1;
                }
                break;
            case BLT:
                if ((signed)R[in.rs1] < (signed)R[in.rs2]) {
                    program_counter += in.imm;
                    jumped = 1;
                }
                break;
            case BLTU:
                if ((unsigned)R[in.rs1] < (unsigned)R[in.rs2]) {
                    program_counter += in.imm;
                    jumped = 1;
                }
                break;
            case BGE:
                if ((signed)R[in.rs1] >= (signed)R[in.rs2]) {
                    program_counter += in.imm << 1;
                    jumped = 1;
                }
                break;
            case BGEU:
                if ((unsigned)R[in.rs1] >= (unsigned)R[in.rs2]) {
                    program_counter += in.imm;
                    jumped = 1;
                }
                break;
            case JAL:
                R[in.rd] = program_counter + 4;
                // TODO: check if its (in.imm << 1) or not
                program_counter += in.imm ;
                jumped = 1;
                break;
            case JALR:
                R[in.rd] = program_counter + 4;
                program_counter = R[in.rs1] + in.imm;
                jumped = 1;
                break;
        }


        if (should_terminate || failed_memory)
            break;

        /* if (executed_instructions_count >= 10) */
        /*     break; */

        if (virt_instruction != VIRTUAL_NONE) {
            switch(virt_instruction) {
                case VIRTUAL_NONE: break;
                case VIRTUAL_COUNT: break;
                case VIRTUAL_INVALID: break;
                case VIRTUAL_PRINT_CHAR:
                    putchar((byte)memory[0x800]);
                    break;
                case VIRTUAL_PRINT_SINT:
                    printf("%i", (i32)memory[address_from_virtual(virt_instruction)]);
                    break;
                case VIRTUAL_PRINT_UINT:
                    printf("%x", memory[address_from_virtual(virt_instruction)]);
                    break;
                case VIRTUAL_HALT:
                    printf("CPU Halt Requested\n");
                    free(memory);
                    exit(0);
                    break;
                case VIRTUAL_READ_CHARACTER:
                    // implemented in memory.c
                    break;
                case VIRTUAL_READ_SINT:
                    // implemented in memory.c
                    break;
                case VIRTUAL_DUMP_PC:
                    printf("%x\n", program_counter);
                    break;
                case VIRTUAL_DUMP_REG:
                    register_dump(program_counter, R);
                    break;
                case VIRTUAL_DUMP_MEM_W:
                    printf("%x\n", memory[address_from_virtual(virt_instruction)]);
                    break;
                case VIRTUAL_HEAP_ALLOC:
                    // TODO: add this
                    break;
                case VIRTUAL_HEAP_FREE:
                    // TODO: add this
                    break;
            }
        }

        executed_instructions_count = executed_instructions_count + 1;

        if (!jumped)
            program_counter += 4;
    }

    register_dump(program_counter, &R[0]);
    // OS will do it anyways, but if it counts as a "memory leak" I get a ZERO
    free(memory);
    return 0;
}
