#include "virtual_machine.h"


Instruction instruction_decode(u32 data) {
    Instruction instruction = { INSTRUCTION_INVALID, TYPE_INVALID, (byte)-1, (byte)-1, (byte)-1, (u16)-1, 0 }; // initialize all values, don't want any nasty bugs!
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
        
        if ((instruction.imm >= (2 << 11))) {
            instruction.imm = instruction.imm - (2 << 12);
        }
        instruction.imm_len = 13;
        }
    }
    else {
        instruction.rd   = ((0b00000000000000000000111110000000) & data) >> 7;
    }

    if (type == TYPE_U || type == TYPE_UJ) {
        if (type == TYPE_U) {
        instruction.imm  = ((0b11111111111111111111000000000000) & data) >> 12;
        instruction.imm_len = 32;
        }
        else {
        /* instruction.imm  = ((0b10000000000000000000000000000000) & data) >> 11; */
        /* instruction.imm |= ((0b01111111111000000000000000000000) & data) >> 20; */
        /* instruction.imm |= ((0b00000000000100000000000000000000) & data) >> 9; */
        /* instruction.imm |= ((0b00000000000011111111000000000000) & data) >> 0; */
        instruction.imm  = ((0b10000000000000000000000000000000) & data) >> 11;
        instruction.imm |= ((0b01111111111000000000000000000000) & data) >> 20;
        instruction.imm |= ((0b00000000000100000000000000000000) & data) >> 9;
        instruction.imm |= ((0b00000000000011111111000000000000) & data) >> 0;
        instruction.imm_len = 21;

        if (instruction.imm >= (2 << 19)) {
            instruction.imm = instruction.imm - (2 << 20);
            instruction.imm = instruction.imm << 1;
        }
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

        if (instruction.imm > (2 << 10)) {
            instruction.imm = instruction.imm - (2 << 11);
        }
        instruction.imm_len = 12;
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


VirtualInstructionName virtual_from_address(u32 address) {
    switch(address) {
        case 0x800:
            return VIRTUAL_PRINT_CHAR;
            break;
        case 0x804:
            return VIRTUAL_PRINT_SINT;
            break;
        case 0x808:
            return VIRTUAL_PRINT_UINT;
            break;
        case 0x80C:
            return VIRTUAL_HALT;
            break;
        case 0x812:
            return VIRTUAL_READ_CHARACTER;
            break;
        case 0x816:
            return VIRTUAL_READ_SINT;
            break;
        case 0x820:
            return VIRTUAL_DUMP_PC;
            break;
        case 0x824:
            return VIRTUAL_DUMP_REG;
            break;
        case 0x828:
            return VIRTUAL_DUMP_MEM_W;
            break;
        case 0x830:
            return VIRTUAL_HEAP_ALLOC;
            break;
        case 0x834:
            return VIRTUAL_HEAP_FREE;
            break;
        default:
            return VIRTUAL_INVALID;
    }
}

u32 address_from_virtual(VirtualInstructionName name) {
    switch(name) {
        case VIRTUAL_PRINT_CHAR:
            return 0x800;
            break;
        case VIRTUAL_PRINT_SINT:
            return 0x804;
            break;
        case VIRTUAL_PRINT_UINT:
            return 0x808;
            break;
        case VIRTUAL_HALT:
            return 0x80C;
            break;
        case VIRTUAL_READ_CHARACTER:
            return 0x812;
            break;
        case VIRTUAL_READ_SINT:
            return 0x816;
            break;
        case VIRTUAL_DUMP_PC:
            return 0x820;
            break;
        case VIRTUAL_DUMP_REG:
            return 0x824;
            break;
        case VIRTUAL_DUMP_MEM_W:
            return 0x828;
            break;
        case VIRTUAL_HEAP_ALLOC:
            return 0x830;
            break;
        case VIRTUAL_HEAP_FREE:
            return 0x834;
            break;
        default:
            return 0;
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

