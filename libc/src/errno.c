#include <errno.h>

#if !defined(__STDC_NO_THREADS__)
#include <threads.h>
#endif

__thread_local int __errno = 0;