#include <locale.h>
#include <string.h>
#include <limits.h>

static char* __collate = "C";
static char* __ctype = "C";
static char* __monetary = "C";
static char* __numeric = "C";
static char* __time = "C";

char* setlocale(int category, const char *locale) {
    if(locale != NULL) {
        // TODO: Check valid locale
        if(cmpstr(locale, "C") != 0)
            return NULL;
        switch (category)
        {
        case LC_ALL:
            strcpy(__collate, locale);
            strcpy(__ctype, locale);
            strcpy(__monetary, locale);
            strcpy(__numeric, locale);
            strcpy(__time, locale);
            return locale; // TODO: ??
        case LC_COLLATE:
            strcpy(__collate, locale);
            return __collate;
        case LC_CTYPE:
            strcpy(__ctype, locale);
            return __ctype;
        case LC_MONETARY:
            strcpy(__monetary, locale);
            return __monetary;
        case LC_NUMERIC:
            strcpy(__numeric, locale);
            return __numeric;
        case LC_TIME:
            strcpy(__time, locale);
            return __time;
        default:
            return NULL;
        }
    } else {
        switch (category)
        {
        case LC_ALL:
            return locale; // TODO: ??
        case LC_COLLATE:
            return __collate;
        case LC_CTYPE:
            return __ctype;
        case LC_MONETARY:
            return __monetary;
        case LC_NUMERIC:
            return __numeric;
        case LC_TIME:
            return __time;
        default:
            return NULL;
        }
    }
}

static struct lconv __lconv;

struct lconv* localeconv(void) {
    if (cmpstr(__monetary,"C")) {
        __lconv.decimal_point = ".";
        __lconv.thousands_sep = "";
        __lconv.grouping = "";
        __lconv.mon_decimal_point = "";
        __lconv.mon_thousands_sep = "";
        __lconv.mon_grouping = "";
        __lconv.positive_sign = "";
        __lconv.negative_sign = "";
        __lconv.currency_symbol = "";
        __lconv.frac_digits = CHAR_MAX;
        __lconv.p_cs_precedes = CHAR_MAX;
        __lconv.n_cs_precedes = CHAR_MAX;
        __lconv.p_sep_by_space = CHAR_MAX;
        __lconv.n_sep_by_space = CHAR_MAX;
        __lconv.p_sign_posn = CHAR_MAX;
        __lconv.n_sign_posn = CHAR_MAX;
        __lconv.int_curr_symbol = "";
        __lconv.int_frac_digits = CHAR_MAX;
        __lconv.int_p_cs_precedes = CHAR_MAX;
        __lconv.int_n_cs_precedes = CHAR_MAX;
        __lconv.int_p_sep_by_space = CHAR_MAX;
        __lconv.int_n_sep_by_space = CHAR_MAX;
        __lconv.int_p_sign_posn = CHAR_MAX;
        __lconv.int_n_sign_posn = CHAR_MAX;
    }
    return &__lconv;
}