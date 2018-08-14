#include "commons.h"

#include <ctype.h>

bool strisblank(const char *s)
{
    while (*s) {
        if (!isblank(*s++)) {
            return false;
        }
    }

    return true;
}

int istrcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && tolower(*s1) == tolower(*s2)) {
        s1++, s2++;
    }

    return tolower(*s1) - tolower(*s2);
}
