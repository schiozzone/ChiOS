#ifndef _ERRNO_H
#define _ERRNO_H 1

#include <sys/cdefs.h>

#if defined(__STDC_NO_THREADS__)
#define __thread_local
#else
#define __thread_local thread_local
#endif

#define errno (__errno)
extern __thread_local int __errno;

#define EDOM   1
#define EILSEQ 2
#define ERANGE 3
// TODO: Add posix defined error numbers

#endif //_ERRNO_H