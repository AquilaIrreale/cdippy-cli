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

%{

#include <config.h>

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <readline/readline.h>

#include "commons.h"
#include "board.h"
#include "game.h"

#define PROMPT "> "

#define YY_INPUT(buf, result, max_size)     \
    result = readline_input(buf, max_size)

size_t readline_input(char buf[], size_t max_size);
int recognize_keyword(char *yytext);

#include "parser.h"

%}

%option caseless
%option noyywrap

%%

[ \r\t\v\f\b]+ { ; /* Ignore */ }

\(NC\) {
    yylval.i = NORTH;
    return COAST;
}

\(SC\) {
    yylval.i = SOUTH;
    return COAST;
}

F|FLEET {
    yylval.i = FLEET;
    return UNIT;
}

A|ARMY {
    yylval.i = ARMY;
    return UNIT;
}

BC|B\.C\.|BCE|B\.C\.E\. {
    yylval.i = BC;
    return ERA;
}

AD|A\.D\.|CE|C\.E\. {
    yylval.i = AD;
    return ERA;
}

[0-9]+ {
    sscanf(yytext, "%u", &yylval.u);
    return NUM;
}

[a-z_][a-z0-9_]* {
    int ret = get_nation(yytext);
    if (ret != NO_NATION) {
        yylval.i = ret;
        return NATION;
    }

    ret = get_terr(yytext);
    if (ret != NO_TERR) {
        yylval.i = ret;
        return TERR;
    }

    ret = recognize_keyword(yytext);

    if (ret == UNRECOGNIZED) {
        yylval.s = malloc(sizeof(yytext) + 1);
        strcpy(yylval.s, yytext);
    }

    return ret;
}

.|\n { return *yytext; }

%%

/*
size_t readline_input(char buf[], size_t max_size)
{
    static bool newline = false;
    static char *line = NULL;
    static size_t linelen = 0;
    static size_t pos = 0;

    if (newline) {
        buf[0] = '\n';
        return 1;
    }

    if (line == NULL) {
        do {
            free(line);
            line = readline(PROMPT);
        } while (strisblank(line));

        linelen = strlen(line);
    }

    size_t n;
    sprintf(buf, "%*s%zn", max_size, line, &n);

    if (n <= max_size) {
        free(line);
        line = NULL;

        if (n < max_size) {
            buf[n++] = '\n';
        } else if (linelen == max_size) {
            newline = true;
        }
    } else {
        newline = true;
    }

    size_t ret = strlen(x) + 1;
    free(x);
    return ret;
}
*/

size_t readline_input(char buf[], size_t max_size)
{
    static char *readline_buf = NULL;
    static size_t chars_left = 0;
    static size_t pos = 0;
    static bool newline = false;

    if (newline) {
        buf[0] = '\n';
        return 1;
    }

    if (readline_buf == NULL) {
        do {
            readline_buf = readline(PROMPT);

            if (readline_buf == NULL) {
                putchar('\n');
                return YY_NULL;
            }
        } while (strisblank(readline_buf));

        chars_left = strlen(readline_buf);
        pos = 0;
    }

    if (chars_left <= max_size) {
        strcpy(buf, readline_buf+pos);
        free(readline_buf);
        readline_buf = NULL;

        if (chars_left < max_size) {
            buf[chars_left] = '\n';
        } else {
            newline = true;
        }
        
        return chars_left + 1;
    }

    strncpy(buf, readline_buf+pos, max_size);
    chars_left -= max_size;
    pos += max_size;

    return max_size;
}

int recognize_keyword(char *yytext)
{
    static struct {
        const char *name;
        int code;
    } keywords[] = {
        {"clear",  CLEAR},
        {"delete", DELETE},
        {"list",   LIST},
        {"phase",  PHASE},
        {"reset",  RESET},
        {"run",    RUN},
        {"set",    SET},
        {"state",  STATE},
        {"year",   YEAR}
    };

    size_t i;
    for (i = 0; i < ARRSIZE(keywords); i++) {
        if (istrcmp(keywords[i].name, yytext) == 0) {
            return keywords[i].code;
        };
    };

    return UNRECOGNIZED;
}
