#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/tty.h>

#if defined(__cplusplus)
extern "C" {
#endif

void print_uint32(void* p) {
    uint32_t r = (uint32_t)p;
    for(int i = 0x1c; i >= 0; i -= 4)
    {
        uint8_t t = (r >> i & 0xf);
        t += t<0xa?'0':'a'-0xa;
        terminal_putchar(t);
    }
    terminal_putchar(' ');
}

void kernel_early(void) {
	terminal_initialize();
	createGdt();
	createIdt();
}

void kernel_main(void) {
	terminal_writestring("Hello, kernel World!\n");
	terminal_writestring("\e[25G---==>[ Welcome to \e[91;100mChiOS\e[m ]<==---\n\n\n");
	terminal_writestring("There's support for\tHorizontal Tabs, \vVertical Tabs and \rCarriage Returns!\n");
}

#if defined(__cplusplus)
} //extern "C"
#endif
