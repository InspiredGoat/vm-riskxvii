#include <stdio.h>

#include "util.h"

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
    print_int_as_hex_string(program_counter * 4);
    printf(";\n");

    for (int i = 0; i < REG_COUNT; i++) {
        printf("R[%u] = ", i);
        print_int_as_hex_string(registers[i]);
        printf(";\n");
    }
}

void instruction_print_name(InstructionName name, FILE* f) {
    switch(name) {
        case ADD:
            fprintf(f, "ADD");
            break;
        case ADDI:
            fprintf(f, "ADDI");
            break;
        case SUB:
            fprintf(f, "SUB");
            break;
        case LUI:
            fprintf(f, "LUI");
            break;
        case XOR:
            fprintf(f, "XOR");
            break;
        case XORI:
            fprintf(f, "XORI");
            break;
        case OR:
            fprintf(f, "OR");
            break;
        case ORI:
            fprintf(f, "ORI");
            break;
        case AND:
            fprintf(f, "AND");
            break;
        case ANDI:
            fprintf(f, "ANDI");
            break;
        case SLL:
            fprintf(f, "SLL");
            break;
        case SRL:
            fprintf(f, "SRL");
            break;
        case SRA:
            fprintf(f, "SRA");
            break;
        case LB:
            fprintf(f, "LB");
            break;
        case LH:
            fprintf(f, "LH");
            break;
        case LW:
            fprintf(f, "LW");
            break;
        case LBU:
            fprintf(f, "LBU");
            break;
        case LHU:
            fprintf(f, "LHU");
            break;
        case SB:
            fprintf(f, "SB");
            break;
        case SH:
            fprintf(f, "SH");
            break;
        case SW:
            fprintf(f, "SW");
            break;
        case SLT:
            fprintf(f, "SLT");
            break;
        case SLTI:
            fprintf(f, "SLTI");
            break;
        case SLTU:
            fprintf(f, "SLTU");
            break;
        case SLTIU:
            fprintf(f, "SLTIU");
            break;
        case BEQ:
            fprintf(f, "BEQ");
            break;
        case BNE:
            fprintf(f, "BNE");
            break;
        case BLT:
            fprintf(f, "BLT");
            break;
        case BLTU:
            fprintf(f, "BLTU");
            break;
        case BGE:
            fprintf(f, "BGE");
            break;
        case BGEU:
            fprintf(f, "BGEU");
            break;
        case JAL:
            fprintf(f, "JAL");
            break;
        case JALR:
            fprintf(f, "JALR");
            break;
        default:
            fprintf(f, "INVALID");
            break;
    }
}

void instruction_print_summary(Instruction instruction, FILE* f) {
    instruction_print_name(instruction.name, f);

    fprintf(f, "\t");
    if (instruction.rd != 255)
        fprintf(f, "%i", instruction.rd);
    fprintf(f, "\t");
    if (instruction.rs1 != 255)
        fprintf(f, "%i", instruction.rs1);
    fprintf(f, "\t");
    if (instruction.rs2 != 255)
        fprintf(f, "%i", instruction.rs2);
    fprintf(f, "\t");
    if (instruction.imm != (u16) (-1))
        fprintf(f, "%i", instruction.imm);
    fprintf(f, "\n");
}
