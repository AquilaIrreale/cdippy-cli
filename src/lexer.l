%{

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

#include <config.h>

#include <stdbool.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <readline/readline.h>

#include "parser.h"

#define PROMPT "> "

#define YY_INPUT(buf, result, max_size)     \
{                                           \
    result = readline_input(buf, max_size); \
}

size_t readline_input(char buf[], size_t max_size);
int parse_keyword(yytext);

%}

%noyywrap

%%

[ \r\t\v\f\b]+ { ; /* Ignore */ }

[Aa][Uu][Ss][Tt][Rr][Ii][Aa]|\
[Ee][Nn][Gg][Ll][Aa][Nn][Dd]|\
[Ff][Rr][Aa][Nn][Cc][Ee]|\
[Gg][Ee][Rr][Mm][Aa][Nn][Yy]|\
[Ii][Tt][Aa][Ll][Yy]|\
[Rr][Uu][Ss][Ss][Ii][Aa]|\
[Tt][Uu][Rr][Kk][Ee][Yy] {
    yylval.i = parse_nation(yytext);
    return NATION;
}

[Aa][Dd][Rr]|[Aa][Ee][Gg]|[Aa][Ll][Bb]|[Aa][Nn][Kk]|\
[Aa][Pp][Uu]|[Aa][Rr][Mm]|[Bb][Aa][Ll]|[Bb][Aa][Rr]|\
[Bb][Ee][Ll]|[Bb][Ee][Rr]|[Bb][Ll][Aa]|[Bb][Oo][Hh]|\
[Bb][Oo][Tt]|[Bb][Rr][Ee]|[Bb][Uu][Dd]|[Bb][Uu][Ll]|\
[Bb][Uu][Rr]|[Cc][Ll][Yy]|[Cc][Oo][Nn]|[Dd][Ee][Nn]|\
[Ee][Aa][Ss]|[Ee][Dd][Ii]|[Ee][Nn][Gg]|[Ff][Ii][Nn]|\
[Gg][Aa][Ll]|[Gg][Aa][Ss]|[Gg][Rr][Ee]|[Hh][Ee][Ll]|\
[Hh][Oo][Ll]|[Ii][Oo][Nn]|[Ii][Rr][Ii]|[Kk][Ii][Ee]|\
[Ll][Oo][Nn]|[Ll][Vv][Nn]|[Ll][Vv][Pp]|[Ll][Yy][Oo]|\
[Mm][Aa][Oo]|[Mm][Aa][Rr]|[Mm][Oo][Ss]|[Mm][Uu][Nn]|\
[Nn][Aa][Ff]|[Nn][Aa][Oo]|[Nn][Aa][Pp]|[Nn][Tt][Hh]|\
[Nn][Ww][Gg]|[Nn][Ww][Yy]|[Pp][Aa][Rr]|[Pp][Ii][Cc]|\
[Pp][Ii][Ee]|[Pp][Oo][Rr]|[Pp][Rr][Uu]|[Rr][Oo][Mm]|\
[Rr][Uu][Hh]|[Rr][Uu][Mm]|[Ss][Ee][Rr]|[Ss][Ee][Vv]|\
[Ss][Ii][Ll]|[Ss][Kk][Aa]|[Ss][Mm][Yy]|[Ss][Pp][Aa]|\
[Ss][Tt][Pp]|[Ss][Ww][Ee]|[Ss][Yy][Rr]|[Tt][Rr][Ii]|\
[Tt][Uu][Nn]|[Tt][Uu][Ss]|[Tt][Yy][Rr]|[Tt][Yy][Ss]|\
[Uu][Kk][Rr]|[Vv][Ee][Nn]|[Vv][Ii][Ee]|[Ww][Aa][Ll]|\
[Ww][Aa][Rr]|[Ww][Ee][Ss]|[Yy][Oo][Rr] {
    yylval.i = parse_terr(yytext);
    return TERR;
}

\([Nn][Cc]\) {
    yylval.i = NC;
    return COAST;
}

\([Ss][Cc]\) {
    yylval.i = SC;
    return COAST;
}

[Bb][Cc]|[Bb]\.[Cc]\.|[Bb][Cc][Ee]|[Bb]\.[Cc]\.[Ee]\. {
    yylval.i = -1;
    return ERA;
}

[Aa][Dd]|[Aa]\.[Dd]\.|[Cc][Ee]|[Cc]\.[Ee]\. {
    yylval.i = 1;
    return ERA;
}

[0-9]+ {
    sscanf(yytext, "%u", &yylval.u);
    return NUM;
}

[A-Za-z]+ {
    return parse_keyword(yytext);
}

.|\n { return *yytext; }

%%

int parse_keyword(yytext)
{
    static struct {
        char *name;
        int code;
    } commands = {
        {"DELETE", DELETE},
        {"PHASE",  PHASE},
        {"RESET",  RESET},
        {"SET",    SET},
        {"YEAR",   YEAR}
    };

    size_t i;
    for (i = 0; i < ARRSIZE(commands); i++) {
        if (strcmp(commands[i].name, yytext) == 0) {
            return commands[i].code;
        };
    };
}

/*
int parse_keyword(yytext)
{
    static struct {
        char *name;
        int code;
        bool valid;
    } commands = {
        {"DELETE", DELETE, true},
        {"PHASE",  PHASE,  true},
        {"RESET",  RESET,  true},
        {"SET",    SET,    true},
        {"YEAR",   YEAR,   true}
    };

    size_t i;
    for (i = 0; i < ARRSIZE(commands); i++) {
    }
    
    return 0;
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
        readline_buf = readline(PROMPT)

        if (readline_buf == NULL) {
            return YY_NULL;
        }

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
        
        return chars_left;
    }

    strncpy(buf, readline_buf+pos, max_size);
    chars_left -= max_size;
    pos += max_size;

    return max_size;
}
