#include <ctype.h>
#include <locale.h>
#include <string.h>

#define _True (-1)
#define _False (0)

// TODO: Add locale support

int isalnum(int c) {
	return (isalpha(c) || isdigit(c)) ? _True : _False;
}

int isalpha(int c) {
	return (isupper(c) || islower(c)) ? _True : _False;
}

int isblank(int c) {
	return (c == ' ' || c == '\t') ? _True : _False;
}

int iscntrl(int c) {
	return (c >= 0 && c <= 0x1F || c == 0x7F) ? _True : _False;
}

int isdigit(int c) {
	return (c >= '0' && c <= '9') ? _True : _False;
}

int isgraph(int c) {
	return (c != ' ' && isprint(c)) ? _True : _False;
}

int islower(int c) {
	return (c >= 'a' && c <= 'z') ? _True : _False;
}

int isprint(int c) {
	return (c >= 0x20 && c <= 0x7E) ? _True : _False;
}

int ispunct(int c) {
	return (!(isspace(c) || isalnum(c))) ? _True : _False;
}

int isspace(int c) {
	return (c == ' ' || c == '\f'|| c == '\n'|| c == '\r'|| c == '\t'|| c == '\v') ? _True : _False;
}

int isupper(int c) {
	return (c >= 'A' && c <= 'Z') ? _True : _False;
}

int isxdigit(int c) {
	return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) ? _True : _False;
}

int tolower(int c) {
	int l = c ^ 0x20;
    return (isupper(c) && islower(l)) ? l : c;
}

int toupper(int c) {
	int u = c ^ 0x20;
    return (islower(c) && isupper(u)) ? u : c;
}
