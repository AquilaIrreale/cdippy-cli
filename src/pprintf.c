/*  cdippy-cli - Keeps track of a game of Diplomacy, and adjudicates
 *               orders automatically
 *
 *  Copyright (C) 2018  Simone Cimarelli a.k.a. AquilaIrreale
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "pprintf.h"

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
    fputs(PPRINTF_PROMPT, stdout);

    struct termios flags;
    tcgetattr(STDIN_FILENO, &flags);

    flags.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &flags);

    while (getchar() != '\n');

    flags.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &flags);

    size_t i;
    for (i = 0; i < strlen(PPRINTF_PROMPT); i++) {
        fputs("\b \b", stdout);
    }

    putchar('\r');

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

int pputchar(int c)
{
    if (c == '\0') {
        return '\0';
    }

    if (pprintf_r + 1 >= pprintf_h) {
        pprintf_block();
    }

    int ret = putchar('\n');

    if (ret == '\n' || pprintf_c + 1 == pprintf_w) {
        pprintf_c = 0;
        pprintf_r++;
    }

    return ret;
}
