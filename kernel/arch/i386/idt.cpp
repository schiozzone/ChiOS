#include <kernel/idt.h>

static inline void loadIdt(idt_ptr* ptr) {
    __asm__("lidt %0" :: "m"(*ptr));
}

idt_entry create_descriptor(uint16_t selector, uint32_t offset, idt_flag flags) {
    idt_entry descriptor;
    descriptor.offset_low = offset & 0xFFFF;
    descriptor.selector = selector;
    descriptor.zero = 0;
    descriptor.type_attr = (uint8_t)flags;
    descriptor.offset_high = offset >> 16;
    return descriptor;
}

static idt_entry IDT[256];
static idt_ptr   IDTR;

void createIdt(void) {
    //memset
    IDTR.base = (uint32_t)&IDT[0];
    IDTR.limit = (uint16_t)(sizeof(idt_entry)*256 - 1);
    loadIdt(&IDTR);
}