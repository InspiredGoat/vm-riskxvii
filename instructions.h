#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "types.h"

// using these macros to avoid typos
#define REG_COUNT 32

#define INSTRUCTION_MEMORY_SIZE 1024
#define DATA_MEMORY_SIZE 1024
#define MAX_BANKS 128

typedef enum {
    INSTRUCTION_INVALID=-1,
    ADD,
    ADDI,
    SUB,
    LUI,
    XOR,
    XORI,
    OR,
    ORI,
    AND,
    ANDI,
    SLL,
    SRL,
    SRA,
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,
    SLT,
    SLTI,
    SLTU,
    SLTIU,
    BEQ,
    BNE,
    BLT,
    BLTU,
    BGE,
    BGEU,
    JAL,
    JALR
} InstructionName;

typedef enum {
    TYPE_INVALID=-1,
    TYPE_R,
    TYPE_I,
    TYPE_S,
    TYPE_SB,
    TYPE_U,
    TYPE_UJ
} InstructionType;

// parse first then perform operation
typedef struct {
    InstructionName name;
    InstructionType type;
    byte rd;
    byte rs1;
    byte rs2;
    u16 imm;
} Instruction;



#endif
