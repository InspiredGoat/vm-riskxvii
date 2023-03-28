#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "virtual_machine.h"


// Returns 0 if invalid memory, 1 if valid, 2 if a virtual isntruction was called
byte set_memory(void* memory, byte* avail_blocks, i32 address, void* data, u32 length, VirtualInstructionName* ret_virt_instruction) {
    *ret_virt_instruction = VIRTUAL_NONE;
    if (address < 0)
        address = (2 << 11) + address;

    if (address < 0x0400 || address + length > (0xD700)) {
        return 0;
    }
    // handle memory bank
    else if (address >= (0xB700)) {
        printf("MEMORY BANK, NOT HANDLED\n\n\n\n");
        // check the memory bank is enabled
        if ((avail_blocks >> (address - 0xB700 / 64)) & 1) {
            return 0;
        }
    }
    // handle virtual functions
    if (address >= 0x0800 && address <= 0x0834) {
        *ret_virt_instruction = virtual_from_address(address);
    }

    memcpy(memory + address, data, (unsigned long) length);

    // check if 
    return 1;
}

byte set_mem_byte(void* memory, byte* avail_blocks, i32 address, byte value, VirtualInstructionName* ret_virt_instruction) {
    return set_memory(memory, avail_blocks, address, &value, 1, ret_virt_instruction);
}
byte set_mem_u16(void* memory, byte* avail_blocks, i32 address, u16 value, VirtualInstructionName* ret_virt_instruction) {
    return set_memory(memory, avail_blocks, address, &value, 2, ret_virt_instruction);
}
byte set_mem_u32(void* memory, byte* avail_blocks, i32 address, u32 value, VirtualInstructionName* ret_virt_instruction) {
    return set_memory(memory, avail_blocks, address, &value, 4, ret_virt_instruction);
}

u32 get_memory(void* memory, byte* avail_blocks, i32 address, void* ret_data, u32 length) {

    if (address < 0)
        address = (2 << 11) + address;

    if (address < 0x000 || address + length > (0xD700))
        return 0;
    // handle memory bank
    else if (address >= (0xB700)) {
        // check the memory bank is enabled
        // TODO: check membank
    }

    if (address == 0x0812) {
        return getchar();
    }
    if (address == 0x0816) {
        char buffer[20];
        memzero(buffer, 20);
        fgets(&(buffer[0]), 20, stdin);
        i32 res = atoi(buffer);

        memcpy(ret_data, &res, 4);
        return 1;
    }

    memcpy(ret_data, memory + address, length);

    return 1;
}

u32 get_mem_char(void* memory, byte* avail_blocks, i32 address, char* ret_value) {
    return get_memory(memory, avail_blocks, address, ret_value, 1);
}
u32 get_mem_byte(void* memory, byte* avail_blocks, i32 address, byte* ret_value) {
    return get_memory(memory, avail_blocks, address, ret_value, 1);
}
u32 get_mem_i16(void* memory, byte* avail_blocks, i32 address, i16* ret_value) {
    return get_memory(memory, avail_blocks, address, ret_value, 2);
}
u32 get_mem_u16(void* memory, byte* avail_blocks, i32 address, u16* ret_value) {
    return get_memory(memory, avail_blocks, address, ret_value, 2);
}
u32 get_mem_i32(void* memory, byte* avail_blocks, i32 address, i32* ret_value) {
    return get_memory(memory, avail_blocks, address, ret_value, 4);
}
u32 get_mem_u32(void* memory, byte* avail_blocks, i32 address, u32* ret_value) {
    return get_memory(memory, avail_blocks, address, ret_value, 4);
}

