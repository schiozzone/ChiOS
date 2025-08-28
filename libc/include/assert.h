#ifndef _ASSERT_H
#define _ASSERT_H 1

#if defined(__cplusplus)
extern "C" {
#endif

void __failed_assertion(const char*, const char*, const char*, const unsigned int);

#if defined(__cplusplus)
} //extern "C"
#endif

#endif //_ASSERT_H

#ifdef NDEBUG

#undef assert
#define assert(ignore) ((void)0)

#else //NDEBUG

#undef assert
#define assert(assertion) ((void)( \
    (assertion) || (__failed_assertion(#assertion, __func__, __FILE__, __LINE__), 0) ))

#endif //NDEBUG

#define static_assert _Static_assert