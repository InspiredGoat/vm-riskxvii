#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "types.h"
#include "util.h"
#include "instructions.h"

typedef struct {
    byte data[64];
} MemoryBank;


Instruction instruction_decode(u32 data) {
    Instruction instruction = { INSTRUCTION_INVALID, TYPE_INVALID, (byte)-1, (byte)-1, (byte)-1, (u16)-1 }; // initialize all values, don't want any nasty bugs!
    InstructionType type = TYPE_INVALID;
    byte opcode, func3, func7 = 0;
        
    opcode               = ((0b00000000000000000000000001111111) & data);
    func3                = ((0b00000000000000000111000000000000) & data) >> 12;
    func7                = ((0b11111110000000000000000000000000) & data) >> 25;

    // Check instruction type
    if (opcode == 0b0100011)
        type = TYPE_S;
    else if (opcode == 0b1100011)
        type = TYPE_SB;
    else if (opcode == 0b0110111)
        type = TYPE_U;
    else if (opcode == 0b1101111)
        type = TYPE_UJ;
    else if (opcode == 0b0010011 ||
             opcode == 0b0000011 ||
             opcode == 0b1100111)
        type = TYPE_I;
    else if (opcode == 0b0110011)
        type = TYPE_R;
    else
        return instruction; // opcode is invalid
    instruction.type = type;

    // Get data for each type
    if (type == TYPE_S || type == TYPE_SB) {
        if (type == TYPE_S) {
        instruction.imm  = ((0b00000000000000000000111110000000) & data) >> 7;
        instruction.imm |= ((0b11111110000000000000000000000000) & data) >> 20;
        }
        else {
        instruction.imm  = ((0b00000000000000000000111100000000) & data) >> 7;
        instruction.imm |= ((0b00000000000000000000000010000000) & data) << 4;
        instruction.imm |= ((0b10000000000000000000000000000000) & data) >> 19;
        instruction.imm |= ((0b01111110000000000000000000000000) & data) >> 20;
        }
    }
    else {
        instruction.rd   = ((0b00000000000000000000111110000000) & data) >> 7;
    }

    if (type == TYPE_U || type == TYPE_UJ) {
        if (type == TYPE_U) {
        instruction.imm  = ((0b11111111111111111111000000000000) & data) >> 12;
        }
        else {
        instruction.imm  = ((0b10000000000000000000000000000000) & data) >> 11;
        instruction.imm |= ((0b01111111111000000000000000000000) & data) >> 20;
        instruction.imm |= ((0b00000000000100000000000000000000) & data) >> 9;
        instruction.imm |= ((0b00000000000011111111000000000000) & data) >> 0;
        }
    }
    else {
        instruction.rs1  = ((0b00000000000011111000000000000000) & data) >> 15;
    }

    if (type != TYPE_U && type != TYPE_UJ && type != TYPE_I) {
        instruction.rs2 = ((0b00000001111100000000000000000000) & data) >> 20;
    }
    else if (type == TYPE_I) {
        instruction.imm = ((0b11111111111100000000000000000000) & data) >> 20;
    }

    // Find instruction names

    if (opcode == 0b1100111) {
        instruction.name = JALR;
    }
    else if (type == TYPE_U) {
        instruction.name = LUI;
    }
    else if (type == TYPE_UJ) {
        instruction.name = JAL;
    }
    else if (opcode == 0b0000011) {
        switch (func3) {
            case 0b000:
                instruction.name = LB;
                break;
            case 0b001:
                instruction.name = LH;
                break;
            case 0b010:
                instruction.name = LW;
                break;
            case 0b100:
                instruction.name = LBU;
                break;
            case 0b101:
                instruction.name = LHU;
                break;
        }
    }
    else if (opcode == 0b0000011) {
        switch (func3) {
            case 0b000:
                instruction.name = LB;
                break;
            case 0b001:
                instruction.name = LH;
                break;
            case 0b010:
                instruction.name = LW;
                break;
            case 0b100:
                instruction.name = LBU;
                break;
            case 0b101:
                instruction.name = LHU;
                break;
        }
    }
    else if (type == TYPE_R || type == TYPE_I) {
        switch (func3) {
        case 0b000:
            // add or sum
            if (func7 == 0b0000000 || type == TYPE_I)
                instruction.name = (type == TYPE_I) ? ADDI : ADD;
            else
                instruction.name = SUB;
            break;
        case 0b100:
            // XOR    
            instruction.name = (type == TYPE_I) ? XORI : XOR;
            break;
        case 0b110:
            // OR 
            instruction.name = (type == TYPE_I) ? ORI : OR;
            break;
        case 0b111:
            // AND 
            instruction.name = (type == TYPE_I) ? ANDI : AND;
            break;
        case 0b010:
            // sll 
            instruction.name = (type == TYPE_I) ? SLTI : SLT;
            break;
        case 0b001:
            // sll 
            instruction.name = SLL;
            break;
        case 0b101:
            // srl
            if (func7 == 0b0000000)
                instruction.name = SRL;
            else
                instruction.name = SRA;
            break;
        case 0b011:
            instruction.name = (type == TYPE_I) ? SLTIU : SLTU;
            break;
        }
    }
    else if (type == TYPE_S) {
        switch (func3) {
            case 0b000:
                instruction.name = SB;
                break;
            case 0b001:
                instruction.name = SH;
                break;
            case 0b010:
                instruction.name = SW;
                break;
        }
    }
    else if (type == TYPE_SB) {
        switch (func3) {
            case 0b000:
                instruction.name = BEQ;
                break;
            case 0b001:
                instruction.name = BNE;
                break;
            case 0b100:
                instruction.name = BLT;
                break;
            case 0b110:
                instruction.name = BLTU;
                break;
            case 0b101:
                instruction.name = BGE;
                break;
            case 0b111:
                instruction.name = BGEU;
                break;
        }
    }

    return instruction;
}


byte set_memory(void* memory, byte mem_bank_bits, u32 address, void* data, u32 length) {
    if (address < 0x400 | address > (0xD700))
        return 0;
    else if (address + length > (0xD700))
        return 0;
    else if (address > (0xB700)) {
    }

    // check if 
    return 1;
}

byte get_memory(void* ret_memory, u32 address, void* data, u32 length) {

    // check if 
    return 1;
}


int main(int argc, char** argv) {

    /* if (argc != 2) { */
    /*     printf("Error, no input file\n"); */
    /*     return 1; */
    /* } */


    byte should_terminate = 0;
    u32 program_counter = 0;
    const u32 R[REG_COUNT];
    byte* memory = malloc(INSTRUCTION_MEMORY_SIZE + DATA_MEMORY_SIZE + 0xff + (128 * 64));

    u32*  instruction_memory  = (u32*) &memory[0];
    byte* data_memory         = &memory[INSTRUCTION_MEMORY_SIZE];
    byte* dynamic_memory      = &memory[0xb700];
    MemoryBank* dynamic_banks = (MemoryBank*) &memory[0xb700];
    byte dynamic_banks_bit_array = 0; // stores whether or not a bank is used as a bit.

    // Scope to avoid using input_file pointer again
    /* { */
    /*     FILE* input_file = fopen(argv[1], "r"); */
    /*     if (input_file == NULL) { */
    /*         printf("Failed to open file: %s\n", argv[1]); */
    /*         free(memory); */
    /*         return 1; */
    /*     } */

    /*     unsigned int bytes_read_length = 0; */

    /*     bytes_read_length  = fread(instruction_memory, 1, INSTRUCTION_MEMORY_SIZE, input_file); */
    /*     bytes_read_length += fread(instruction_memory, 1, DATA_MEMORY_SIZE, input_file); */

    /*     fclose(input_file); */
    /*     if (bytes_read_length != INSTRUCTION_MEMORY_SIZE + DATA_MEMORY_SIZE) { */
    /*         printf("Binary blob in invalid format, expected 2048 bytes got %i.\n", bytes_read_length); */
    /*         free(memory); */
    /*         return 1; */
    /*     } */
    /* } */

    /*           opcode        00000000000000000000000001111111; */
    /*           rd            00000000000000000000111110000000; */
    /*           rs1           00000000000011111000000000000000; */
    /*           func3         00000000000000000111000000000000; */
    instruction_memory[0]  = 0x69696969;
    instruction_memory[1]  = 0x00c000ef;
    instruction_memory[2]  = 0x000017b7;
    instruction_memory[3]  = 0x80078623;

    instruction_memory[4]  = 0x00001737;
    instruction_memory[5]  = 0x81670793;
    instruction_memory[6]  = 0x0007a683;
    instruction_memory[7]  = 0x0007a783;
    instruction_memory[8]  = 0x00d787b3;
    instruction_memory[9]  = 0x80f72223;
    instruction_memory[10] = 0x00000513;
    instruction_memory[11] = 0x00008067;

#ifdef DEBUG_PRINT_INSTRUCTIONS
    printf("Code\t\tName\tRd\tR1\tR2\tImm\n");
#endif

    while(program_counter < INSTRUCTION_MEMORY_SIZE) {
        Instruction instruction;
        u32 instruction_data = instruction_memory[program_counter];

        instruction = instruction_decode(instruction_data);

#ifdef DEBUG_PRINT_INSTRUCTIONS
        print_int_as_hex_string(instruction_data);
        instruction_print_summary(instruction, stdout);
#endif


        switch (instruction.name) {
            case INSTRUCTION_INVALID:
                printf("Instruction Not Implemented: ");
                print_int_as_hex_string(instruction_data);
                printf("\n");
                should_terminate = 1;

                break;
            case ADD:
                set_memory();
                break;
        }

        if (should_terminate)
            break;

        if (program_counter > 10)
            break;

        program_counter += 1;
    }

    register_dump(program_counter, &R[0]);
    // OS will do it anyways, but if it counts as a "memory leak" I get a ZERO
    free(memory);
    return 0;
}
