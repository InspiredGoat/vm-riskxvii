#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <inttypes.h>

/* -------------------------------     Constants     ------------------------------- */\

#define REG_COUNT 32

#define INSTRUCTION_MEMORY_SIZE 1024
#define DATA_MEMORY_SIZE 1024
#define MAX_BANKS 128

#define TOTAL_MEM_SIZE INSTRUCTION_MEMORY_SIZE + DATA_MEMORY_SIZE + 0xff + (128 * 64)

#define R_I32_FLAG 0b000
#define R_U32_FLAG 0b001
#define R_I16_FLAG 0b010
#define R_U16_FLAG 0b011
#define R_U8_FLAG  0b100
#define R_I8_FLAG  0b101


/* -------------------------------       Types       ------------------------------- */

typedef unsigned char   byte;
typedef unsigned char   u8;
typedef char            i8;
typedef uint16_t        u16;
typedef int16_t         i16;
typedef uint32_t        u32;
typedef int32_t         i32;
typedef uint64_t        u64;
typedef int64_t         i64;

#define R_CAST(value, type_flag) (R_I32_FLAG == type_flag) * ((i32)value) + \
                                 (R_U32_FLAG == type_flag) * ((u32)value) + \
                                 (R_I16_FLAG == type_flag) * ((i16)value) + \
                                 (R_U16_FLAG == type_flag) * ((u16)value) + \
                                 (R_I8_FLAG  == type_flag) * ((i8)value) + \
                                 (R_U8_FLAG  == type_flag) * ((u8)value)

/* ------------------------------- Instruction types ------------------------------- */

typedef enum {
    INSTRUCTION_INVALID=-1, 
    // arithmetic and logic
    ADD, ADDI, SUB, LUI, XOR, XORI, OR, ORI, AND, ANDI, SLL, SRL, SRA, 
    // memory
    LB, LH, LW, LBU, LHU, SB, SH, SW, 
    // program flow
    SLT, SLTI, SLTU, SLTIU, BEQ, BNE, BLT, BLTU, BGE, BGEU, JAL, JALR,
    INSTRUCTION_COUNT
} InstructionName;

typedef enum {
    VIRTUAL_INVALID=-1,
    VIRTUAL_NONE=0,
    VIRTUAL_PRINT_CHAR,
    VIRTUAL_PRINT_SINT,
    VIRTUAL_PRINT_UINT,
    VIRTUAL_HALT,
    VIRTUAL_READ_CHARACTER,
    VIRTUAL_READ_SINT,
    VIRTUAL_DUMP_PC,
    VIRTUAL_DUMP_REG,
    VIRTUAL_DUMP_MEM_W,
    VIRTUAL_HEAP_ALLOC,
    VIRTUAL_HEAP_FREE,
    VIRTUAL_COUNT
} VirtualInstructionName;

typedef enum {
    TYPE_INVALID=-1,
    TYPE_R,
    TYPE_I,
    TYPE_S,
    TYPE_SB,
    TYPE_U,
    TYPE_UJ
} InstructionType;

typedef struct {
    InstructionName name;
    InstructionType type;
    byte rd;
    byte rs1;
    byte rs2;
    i32 imm;
    i32 imm_len;
} Instruction;


/* -------------------------------  Printing  stuff  ------------------------------- */

void print_int_as_hex_string(u32 integer);

void register_dump(u32 program_counter, const u32* registers);
void register_dump_text(char* buffer, u32 program_counter, const u32* registers);
void instruction_print_name(InstructionName name, FILE* file);
void instruction_print_summary(Instruction instruction, FILE* file);


/* -------------------------------    Instruction    ------------------------------- */

Instruction instruction_decode(u32 data);
VirtualInstructionName virtual_from_address(u32 address);
u32 address_from_virtual(VirtualInstructionName name);


/* -------------------------------      Memory       ------------------------------- */ 

/* Returns 0 if invalid memory, 1 if valid, 2 if a virtual instruction was called */
byte set_mem(void* memory, byte mem_bank_bits, i32 address, void* data, u32 length, VirtualInstructionName* ret_virt_instruction);
byte set_mem_byte(void* memory, byte mem_bank_bits, i32 address, byte value, VirtualInstructionName* ret_virt_instruction);
byte set_mem_u16(void* memory, byte mem_bank_bits, i32 address, u16 value, VirtualInstructionName* ret_virt_instruction);
byte set_mem_u32(void* memory, byte mem_bank_bits, i32 address, u32 value, VirtualInstructionName* ret_virt_instruction);

u32 get_mem(void* memory, byte mem_bank_bits, i32 address, void* ret_data, u32 length);
u32 get_mem_char(void* memory, byte mem_bank_bits, i32 address, char* ret_value);
u32 get_mem_byte(void* memory, byte mem_bank_bits, i32 address, byte* ret_value);
u32 get_mem_i16(void* memory, byte mem_bank_bits, i32 address, i16* ret_value);
u32 get_mem_u16(void* memory, byte mem_bank_bits, i32 address, u16* ret_value);
u32 get_mem_i32(void* memory, byte mem_bank_bits, i32 address, i32* ret_value);
u32 get_mem_u32(void* memory, byte mem_bank_bits, i32 address, u32* ret_value);



#endif
