#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void __failed_assertion(const char* assertion, const char* func, const char* file, const unsigned int line) {
    fprintf(stderr, "Assert failed: %s, %s(%s:%d)\n", assertion, func, file, line);
    abort();
}