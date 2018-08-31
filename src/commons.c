#include <unistd.h>
#include <sys/ioctl.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <readline/readline.h>

#include "commons.h"

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

size_t decimal_places(size_t n)
{
    size_t ret = 1;

    while (n /= 10) {
        ret++;
    }

    return ret;
}

int size_t_cmp(const void *x, const void *y)
{
    size_t a = *(const size_t *)x;
    size_t b = *(const size_t *)y;

    return a < b ? -1
         : a > b ? 1
         : 0;
}

static unsigned short pprintf_h;
static unsigned short pprintf_w;
static unsigned short pprintf_r;
static unsigned short pprintf_c;

void pprintf_init()
{
    struct winsize ws;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);

    pprintf_h = ws.ws_row;
    pprintf_w = ws.ws_col;
    pprintf_r = 0;
    pprintf_c = 0;
}

static void pprintf_block()
{
    char *ignore = readline("--MORE--");
    while (ignore == NULL) {
        ignore = readline(NULL);
    }

    free(ignore);

    pprintf_init();
}

int pprintf(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    int len = vsnprintf(NULL, 0, format, ap);
    va_end(ap);

    char *buf = malloc((len + 1) * sizeof *buf);

    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);

    char *line = buf;
    char *end;

    do {
        end = strchr(line, '\n');

        if (end != NULL) {
            *end = '\0';
        }

        do {
            if (pprintf_r + 1 >= pprintf_h && line[0] != '\0') {
                pprintf_block();
            }

            assert(pprintf_c < pprintf_w);
            short space_left = pprintf_w - pprintf_c;

            unsigned short written = printf("%.*s", space_left, line);
            if (written == space_left || end != NULL) {
                if (end != NULL) {
                    putchar('\n');
                }

                pprintf_c = 0;
                pprintf_r++;
            } else {
                pprintf_c += written;
            }

            line += written;
        } while (line[0] != '\0');

        line = end + 1;
    } while (end != NULL);

    free(buf);

    return len;
}
