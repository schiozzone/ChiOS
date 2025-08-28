#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel/tty.h>

#include "vga.h"

static constexpr size_t VGA_WIDTH = 80;
static constexpr size_t VGA_HEIGHT = 25;
static constexpr uintptr_t VGA_MEMORY = 0xB8000;

static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
static volatile uint16_t* terminal_buffer = (uint16_t*) VGA_MEMORY;

void terminal_clear(void) {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void inline terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_scroll(int lines) {
	if (lines > 0) {
		for (size_t y = 0; y < VGA_HEIGHT - lines; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t src = (y + lines) * VGA_WIDTH + x;
				const size_t dst = y * VGA_WIDTH + x;
				terminal_buffer[dst] = terminal_buffer[src];
			}
		}
		for (size_t y = VGA_HEIGHT - lines; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t index = y * VGA_WIDTH + x;
				terminal_buffer[index] = vga_entry(' ', terminal_color);
			}
		}
	}
	else if (lines < 0) {
		lines = -lines;
		const size_t size = VGA_HEIGHT * VGA_WIDTH;
		for (size_t y = 0; y < VGA_HEIGHT - lines; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t src = (y + lines) * VGA_WIDTH + x;
				const size_t dst = y * VGA_WIDTH + x;
				terminal_buffer[size - dst] = terminal_buffer[size - src];
			}
		}
		for (size_t y = VGA_HEIGHT - lines; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t index = y * VGA_WIDTH + x;
				terminal_buffer[size - index] = vga_entry(' ', terminal_color);
			}
		}
	}
}

void inline terminal_putentryat(uint16_t entry, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = entry;
}

static enum sequence {NONE, ESC, CSIp, CSIi} terminal_sequence = NONE;
static int CSI_params[2];
static size_t CSI_param_i = 0;

void terminal_putchar(char c) {
	switch(terminal_sequence) {
		case NONE:
		switch(c) {
			// NUL
			case 0x00:
			{
				break;
			}
			// BELl
			case 0x07:
			{
				// Play bell
				break;
			}
			// BackSpace
			case 0x08:
			{
				if(terminal_column)
					terminal_putentryat(
						vga_entry(' ', terminal_color),
						--terminal_column, terminal_row);
				break;
			}
			// Horizontal Tab
			case 0x09:
			{
				terminal_column += 8 - terminal_column % 8;
				break;
			}
			// Line Feed
			case 0x0A:
			{
				terminal_column = 0;
				terminal_row++;
				break;
			}
			// Vertical Tab
			case 0x0B:
			{
				terminal_row += 6 - terminal_row % 6;
				break;
			}
			// Form Feed
			case 0x0C:
			{
				terminal_clear();
				terminal_column = terminal_row = 0;
				break;
			}
			// Carraige Return
			case 0x0D:
			{
				terminal_column = 0;
				break;
			}
			// ESCape
			case 0x1B:
			{
				terminal_sequence = ESC;
				break;
			}
			// Printable ASCII
			case 0x20 ... 0x7E:
			default:
			{
				terminal_putentryat(
					vga_entry(c, terminal_color),
					terminal_column++, terminal_row);
				break;
			}
			// DELete
			case 0x7F:
			{
				terminal_putentryat(
						vga_entry(' ', terminal_color),
						terminal_column, terminal_row);
				break;
			}
		}
		break;
		case ESC:
		switch(c) {
			// Fe
			// case 0x40 ... 0x5F:
			case 0x5B:
			CSI_params[0] = CSI_params[1] = 0;
			CSI_param_i = 0;
			terminal_sequence = CSIp;
			break;
			default:
			// Fp
			case 0x30 ... 0x3F:
			// Fs
			case 0x60 ... 0x7E:
			terminal_sequence = NONE;
			// nF
			case 0x20 ... 0x2F:
			break;
		}
		break;
		case CSIp:
		switch(c) {
			// INTERMEDIATES 0x20 ... 0x2F
			case 0x20 ... 0x2F:

			terminal_sequence = CSIi;
			break;
			// PARAMETERS 0x30 ... 0x3F
			case 0x30 ... 0x39:
			CSI_params[CSI_param_i] *= 10;
			CSI_params[CSI_param_i] += c - '0';
			break;
			case 0x3B:
			CSI_param_i++;
			if (CSI_param_i >= 2) terminal_sequence = NONE;
			break;
			case 0x3A:
			case 0x3C ... 0x3F:
			break;
			// FINAL 0x40 ... 0x7E
			case 0x46:
			terminal_column = 0;
			[[fallthrough]];
			case 0x41:
			{
				size_t n = CSI_params[0] ? CSI_params[0] : 1;
				if (terminal_row > n)
					terminal_row -= n;
				else
					terminal_row = 0;
				terminal_sequence = NONE;
				break;
			}
			case 0x45:
			terminal_column = 0;
			[[fallthrough]];
			case 0x42:
			{
				size_t n = CSI_params[0] ? CSI_params[0] : 1;
				if (terminal_row + n < VGA_HEIGHT)
					terminal_row += n;
				else
					terminal_row = VGA_HEIGHT;
				terminal_sequence = NONE;
				break;
			}
			case 0x43:
			{
				size_t n = CSI_params[0] ? CSI_params[0] : 1;
				if (terminal_column + n < VGA_WIDTH)
					terminal_column += n;
				else
					terminal_column = VGA_WIDTH;
				terminal_sequence = NONE;
				break;
			}
			case 0x44:
			{
				size_t n = CSI_params[0] ? CSI_params[0] : 1;
				if (terminal_column > n)
					terminal_column -= n;
				else
					terminal_column = 0;
				terminal_sequence = NONE;
				break;
			}
			case 0x47:
			{
				size_t n = CSI_params[0] ? CSI_params[0] : 1;
				if (n <= VGA_WIDTH)
					terminal_column = n - 1;
				else
					terminal_column = VGA_WIDTH;
				terminal_sequence = NONE;
				break;
			}
			case 0x66:
			case 0x48:
			{
				size_t n = CSI_params[0] ? CSI_params[0] : 1;
				size_t m = CSI_params[1] ? CSI_params[1] : 1;
				if (n <= VGA_HEIGHT)
					terminal_row = n - 1;
				else
					terminal_row = VGA_HEIGHT;
				if (m <= VGA_WIDTH)
					terminal_column = m - 1;
				else
					terminal_column = VGA_WIDTH;
				terminal_sequence = NONE;
				break;
			}
			case 0x4A:
			{
				size_t n = CSI_params[0];
				switch(n) {
					case 0:
					for(size_t i = terminal_row * VGA_WIDTH + terminal_column;
						i < VGA_WIDTH * VGA_HEIGHT; i++)
						terminal_buffer[i] = vga_entry(' ', terminal_color);
					break;
					case 1:
					for(size_t i = terminal_row * VGA_WIDTH + terminal_column;
						i < VGA_WIDTH * VGA_HEIGHT; i--)
						terminal_buffer[i] = vga_entry(' ', terminal_color);
					break;
					case 2:
					for(size_t i = 0;
						i < VGA_WIDTH * VGA_HEIGHT; i++)
						terminal_buffer[i] = vga_entry(' ', terminal_color);
					break;
				}
				terminal_sequence = NONE;
				break;
			}
			case 0x4B:
			{
				size_t n = CSI_params[0];
				switch(n) {
					case 0:
					for(size_t i = terminal_column;	i < VGA_WIDTH; i++)
						terminal_buffer[terminal_row * VGA_WIDTH + i] = vga_entry(' ', terminal_color);
					break;
					case 1:
					for(size_t i = terminal_column; i < VGA_WIDTH; i--)
						terminal_buffer[terminal_row * VGA_WIDTH + i] = vga_entry(' ', terminal_color);
					break;
					case 2:
					for(size_t i = 0; i < VGA_WIDTH; i++)
						terminal_buffer[terminal_row * VGA_WIDTH + i] = vga_entry(' ', terminal_color);
					break;
				}
				terminal_sequence = NONE;
				break;
			}
			case 0x53:
			{
				size_t n = CSI_params[0] ? CSI_params[0] : 1;
				terminal_scroll(-n);
				terminal_sequence = NONE;
				break;
			}
			case 0x54:
			{
				size_t n = CSI_params[0] ? CSI_params[0] : 1;
				terminal_scroll(n);
				terminal_sequence = NONE;
				break;
			}
			case 0x6D:
			{
				size_t n = CSI_params[0];
				size_t m = CSI_params[1];
				enum vga_color fg, bg;
				fg = (enum vga_color) (terminal_color & 0x0F);
				bg = (enum vga_color) (terminal_color & 0xF0 >> 4);
				switch(n) {
					case 0:
					fg = VGA_COLOR_LIGHT_GREY;
					bg = VGA_COLOR_BLACK;
					break;
					case 1:
					m += 60;
					break;
					case 30:
					fg = VGA_COLOR_BLACK;
					break;
					case 31:
					fg = VGA_COLOR_RED;
					break;
					case 32:
					fg = VGA_COLOR_GREEN;
					break;
					case 33:
					fg = VGA_COLOR_BROWN;
					break;
					case 34:
					fg = VGA_COLOR_BLUE;
					break;
					case 35:
					fg = VGA_COLOR_MAGENTA;
					break;
					case 36:
					fg = VGA_COLOR_CYAN;
					break;
					case 37:
					fg = VGA_COLOR_LIGHT_GREY;
					break;
					case 39:
					fg = VGA_COLOR_LIGHT_GREY;
					break;
					case 40:
					bg = VGA_COLOR_BLACK;
					break;
					case 41:
					bg = VGA_COLOR_RED;
					break;
					case 42:
					bg = VGA_COLOR_GREEN;
					break;
					case 43:
					bg = VGA_COLOR_BROWN;
					break;
					case 44:
					bg = VGA_COLOR_BLUE;
					break;
					case 45:
					bg = VGA_COLOR_MAGENTA;
					break;
					case 46:
					bg = VGA_COLOR_CYAN;
					break;
					case 47:
					bg = VGA_COLOR_LIGHT_GREY;
					break;
					case 49:
					bg = VGA_COLOR_LIGHT_GREY;
					break;
					case 90:
					fg = VGA_COLOR_DARK_GREY;
					break;
					case 91:
					fg = VGA_COLOR_LIGHT_RED;
					break;
					case 92:
					fg = VGA_COLOR_LIGHT_GREEN;
					break;
					case 93:
					fg = VGA_COLOR_LIGHT_BROWN;
					break;
					case 94:
					fg = VGA_COLOR_LIGHT_BLUE;
					break;
					case 95:
					fg = VGA_COLOR_LIGHT_MAGENTA;
					break;
					case 96:
					fg = VGA_COLOR_LIGHT_CYAN;
					break;
					case 97:
					bg = VGA_COLOR_WHITE;
					break;
					case 100:
					bg = VGA_COLOR_DARK_GREY;
					break;
					case 101:
					bg = VGA_COLOR_LIGHT_RED;
					break;
					case 102:
					bg = VGA_COLOR_LIGHT_GREEN;
					break;
					case 103:
					bg = VGA_COLOR_LIGHT_BROWN;
					break;
					case 104:
					bg = VGA_COLOR_LIGHT_BLUE;
					break;
					case 105:
					bg = VGA_COLOR_LIGHT_MAGENTA;
					break;
					case 106:
					bg = VGA_COLOR_LIGHT_CYAN;
					break;
					case 107:
					bg = VGA_COLOR_WHITE;
					break;
					default:
					break;
				}
				switch(m) {
					case 30:
					fg = VGA_COLOR_BLACK;
					break;
					case 31:
					fg = VGA_COLOR_RED;
					break;
					case 32:
					fg = VGA_COLOR_GREEN;
					break;
					case 33:
					fg = VGA_COLOR_BROWN;
					break;
					case 34:
					fg = VGA_COLOR_BLUE;
					break;
					case 35:
					fg = VGA_COLOR_MAGENTA;
					break;
					case 36:
					fg = VGA_COLOR_CYAN;
					break;
					case 37:
					fg = VGA_COLOR_LIGHT_GREY;
					break;
					case 39:
					fg = VGA_COLOR_LIGHT_GREY;
					break;
					case 40:
					bg = VGA_COLOR_BLACK;
					break;
					case 41:
					bg = VGA_COLOR_RED;
					break;
					case 42:
					bg = VGA_COLOR_GREEN;
					break;
					case 43:
					bg = VGA_COLOR_BROWN;
					break;
					case 44:
					bg = VGA_COLOR_BLUE;
					break;
					case 45:
					bg = VGA_COLOR_MAGENTA;
					break;
					case 46:
					bg = VGA_COLOR_CYAN;
					break;
					case 47:
					bg = VGA_COLOR_LIGHT_GREY;
					break;
					case 49:
					bg = VGA_COLOR_LIGHT_GREY;
					break;
					case 90:
					fg = VGA_COLOR_DARK_GREY;
					break;
					case 91:
					fg = VGA_COLOR_LIGHT_RED;
					break;
					case 92:
					fg = VGA_COLOR_LIGHT_GREEN;
					break;
					case 93:
					fg = VGA_COLOR_LIGHT_BROWN;
					break;
					case 94:
					fg = VGA_COLOR_LIGHT_BLUE;
					break;
					case 95:
					fg = VGA_COLOR_LIGHT_MAGENTA;
					break;
					case 96:
					fg = VGA_COLOR_LIGHT_CYAN;
					break;
					case 97:
					bg = VGA_COLOR_WHITE;
					break;
					case 100:
					bg = VGA_COLOR_DARK_GREY;
					break;
					case 101:
					bg = VGA_COLOR_LIGHT_RED;
					break;
					case 102:
					bg = VGA_COLOR_LIGHT_GREEN;
					break;
					case 103:
					bg = VGA_COLOR_LIGHT_BROWN;
					break;
					case 104:
					bg = VGA_COLOR_LIGHT_BLUE;
					break;
					case 105:
					bg = VGA_COLOR_LIGHT_MAGENTA;
					break;
					case 106:
					bg = VGA_COLOR_LIGHT_CYAN;
					break;
					case 107:
					bg = VGA_COLOR_WHITE;
					break;
					default:
					break;
				}
				terminal_setcolor(vga_entry_color(fg, bg));
				terminal_sequence = NONE;
				break;
			}
			case 0x40:
			case 0x49:
			case 0x4C ... 0x52:
			case 0x55 ... 0x65:
			case 0x67 ... 0x6C:
			case 0x6E ... 0x7E:
			// DO STUFF
			default:
			terminal_sequence = NONE;
			break;
		}
		break;
		case CSIi:
		switch(c) {
			// INTERMEDIATES
			case 0x20 ... 0x2F:

			break;
			// FINAL
			case 0x40 ... 0x7E:
			// DO STUFF
			default:
			terminal_sequence = NONE;
			break;
		}
		break;
	}
	if (terminal_column >= VGA_WIDTH)
	{
		terminal_column = 0;
		terminal_row++;
	}
	if (terminal_row >= VGA_HEIGHT)
	{
		terminal_column = 0;
		terminal_scroll(1);
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	for(;*data;data++)
		terminal_putchar(*data);
}

void terminal_initialize(void) {
	terminal_clear();
}