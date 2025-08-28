#include <kernel/gdt.h>

extern "C" void loadGdt(gdt_ptr*, uint16_t, uint16_t);

static gdt_entry create_descriptor(uint32_t base, uint32_t limit, gdt_flag flags) {
    union {
        gdt_entry descriptor;
        uint32_t raw[2];
    };
    raw[0] = limit & 0x0000FFFF;
    raw[0] |= base << 16;
    raw[1] = limit & 0x000F0000;
    raw[1] |= ((uint16_t)flags << 8) & 0x00F0FF00;
    raw[1] |= (base >> 16) & 0x000000FF;
    raw[1] |= base & 0xFF000000;
    return descriptor;
}

static gdt_entry GDT[5];
static gdt_ptr   GDTR;

void createGdt(void) {
    gdt_flag f = gdt_flag::RW | gdt_flag::S | gdt_flag::Pr | gdt_flag::Gr | gdt_flag::Sz;
    GDT[0] = create_descriptor(0, 0, (gdt_flag)0);
    GDT[1] = create_descriptor(0, 0x000FFFFF, f | get_privl_gdtflag(0) | gdt_flag::Ex);
    GDT[2] = create_descriptor(0, 0x000FFFFF, f | get_privl_gdtflag(0));
    GDT[3] = create_descriptor(0, 0x000FFFFF, f | get_privl_gdtflag(3) | gdt_flag::Ex);
    GDT[4] = create_descriptor(0, 0x000FFFFF, f | get_privl_gdtflag(3));
    // GDT[5] = create_descriptor(0, 0, 0); //tss
    GDTR.base = (uint32_t)&GDT[0];
    GDTR.limit = (uint16_t)(sizeof(gdt_entry)*5 - 1);
    loadGdt(&GDTR, create_selector(1, 0), create_selector(2, 0));
}