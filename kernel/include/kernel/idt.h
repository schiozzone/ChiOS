#ifndef ARCH_I386_IDT_H
#define ARCH_I386_IDT_H

#include <stdint.h>

enum class idt_flag : uint8_t {
    T0 = 0x01, // Gate type
    T1 = 0x02, // (Don't use these)
    T2 = 0x04, // (See below)
    T3 = 0x08, // (4 bits)
    S  = 0x10, // Storage type (interrupts and trap gates = 0)
    R0 = 0x20, // Privilege bits (Ring level) 2 bits
    R1 = 0x40, // (0 = highest, 3 = lowest)
    P  = 0x80, // Present bit (0 for unused interrupts)

    // Possible IDT gate types
    task32 = 0x05, // 80386 32 bit task gate
    intr16 = 0x06, // 80286 16 bit interrupt gate
    trap16 = 0x07, // 80286 16 bit trap gate
    intr32 = 0x0E, // 80386 32 bit interrupt gate
    trap32 = 0x0F, // 80386 32 bit trap gate
};

constexpr idt_flag get_privl_idtflag(unsigned int ring_level) {
    return (idt_flag)((ring_level & 0x03) << 5);
}

constexpr idt_flag operator|(const idt_flag a, const idt_flag b) {
    return (idt_flag)(uint16_t(a) | uint16_t(b));
}

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
}__attribute__((packed, aligned(8)));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
}__attribute__((packed, aligned(2)));

void createIdt(void);

#endif //ARCH_I386_IDT_H