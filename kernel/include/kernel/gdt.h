#ifndef ARCH_I386_GDT_H
#define ARCH_I386_GDT_H

#include <stdint.h>

enum class gdt_flag : uint16_t {
    Ac = 0x0001, // Accessed bit (set to 0)
    RW = 0x0002, // Readable bit (code)/Writable bit (data)
    DC = 0x0004, // Direction bit (data)/Conforming bit (code)
    Ex = 0x0008, // Executable bit (code = 1, data = 0)
    S  = 0x0010, // Descriptor type (code or data = 1, system = 0 (eg. tss))
    R0 = 0x0020, // Privilege bits (Ring level) 2 bits
    R1 = 0x0040, // (0 = highest, 3 = lowest)
    Pr = 0x0080, // Present bit (1 for valid sectors)
    Gr = 0x4000, // Granularity bit (0 = 1B, 1 = 4KiB)
    Sz = 0x8000, // Size bit (0 = 16bit, 1 = 32bit)
};

constexpr gdt_flag get_privl_gdtflag(unsigned int ring_level) {
    return (gdt_flag)((ring_level & 0x03) << 5);
}

constexpr gdt_flag operator|(const gdt_flag a, const gdt_flag b) {
    return (gdt_flag)(uint16_t(a) | uint16_t(b));
}

constexpr uint16_t create_selector(uint16_t id, uint16_t ring_level) {
    return id << 3 | (ring_level & 0x0003);
}

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  acess_byte;
    uint8_t  limit_high:4;
    uint8_t  flags:4;
    uint8_t  base_high;
}__attribute__((packed, aligned(8)));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
}__attribute__((packed, aligned(2)));

void createGdt(void);

#endif //ARCH_I386_GDT_H