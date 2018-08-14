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

/* TODO: use bison */

#include <config.h>

#include <stdio.h>
#include <ctype.h>

#include "commons.h"
#include "game.h"
#include "board.h"

void yyerror(const char *s);
int yywrap();
int yylex();

void range_error(unsigned a, unsigned b);

%}

%union {
    int i;
    unsigned u;
    struct terr_coast tc;
    tclist_t tclist;
    struct range r;
    rangelist_t rlist;
    terrlist_t tlist;
    char *s;
    bool b;
}

%token ALL
%token CLEAR
%token DELETE
%token LIST
%token PHASE
%token RESET
%token RUN
%token SET
%token STATE
%token YEAR

%token <i> NATION
%token <i> TERR
%token <i> COAST
%token <i> UNIT
%token <i> ERA
%token <i> SEASON

%token <u> NUM

%token <s> UNRECOGNIZED

%type <i> era

%type <tc> terr_coast
%type <tclist> tclist

%type <r> range
%type <rlist> range_list

%start commands

%destructor { tclist_free($$); } <tclist>
%destructor { rangelist_free($$); } <rlist>
%destructor { free($$); } <s>

%%

commands: /* Nothing */
        | commands command '\n'
        | commands error '\n' { yyerrok; }

command: set
       | order
       | delete
       | clear
       | NATION { select_nation($1); }
       | LIST   { list_orders(); }
       | STATE  { print_board(); }
       | RESET  { board_init(); }
       | RUN    { adjudicate(); }

set: SET tclist UNIT NATION { set_terrs($2, $3, $4); tclist_free($2); }
   | SET YEAR NUM era       { set_year($3 * $4); }
   | SET PHASE SEASON       { set_phase($3); }

clear: CLEAR tclist { clear_terrs($2); tclist_free($2); }
     | CLEAR ALL    { clear_all_terrs(); }

tclist: terr_coast        { $$ = tclist_cons($1); }
      | tclist terr_coast { $$ = tclist_add($1, $2); }

terr_coast: TERR COAST { $$.terr = $1; $$.coast = $2; }
          | TERR       { $$.terr = $1; $$.coast = NONE; }

era: ERA
   | /* Default */ { $$ = AD; }

delete: DELETE range_list { delete_orders($2); rangelist_free($2); }

range_list: range            { $$ = rangelist_cons($1); }
          | range_list range { $$ = rangelist_add($1, $2); }

range: NUM '-' NUM {
    if ($1 > $3) {
        range_error($1, $3);
        YYERROR;
    }

    $$.a = $1;
    $$.b = $3 + 1;
} | NUM {
    $$.a = $1;
    $$.b = $1 + 1;
}

order: /* TODO */

%%

const char *tokenstr(int token)
{
    static struct {
        int code;
        const char *name;
    } keywords[] = {
        {ALL,    "all"},
        {CLEAR,  "clear"},
        {DELETE, "delete"},
        {LIST,   "list"},
        {PHASE,  "phase"},
        {RESET,  "reset"},
        {RUN,    "run"},
        {SET,    "set"},
        {STATE,  "state"},
        {YEAR,   "year"}
    };

    size_t i;
    for (i = 0; i < ARRSIZE(keywords); i++) {
        if (keywords[i].code == token) {
            return keywords[i].name;
        }
    }

    switch (token) {
    case NATION:
        return get_nation_name(yylval.i);

    case TERR:
        return get_terr_name(yylval.i);

    case COAST:
    case UNIT:
    case ERA:
    case SEASON:
        break; /* TODO */
    }

    return "?!";
}

void yyerror(const char *s)
{
    switch (yychar) {
    case YYEMPTY:
        fprintf(stderr, "%s\n", s);
        return;

    case YYEOF:
        fprintf(stderr, "%s: unexpected EOF\n", s);
        return;

    case '\n':
        fprintf(stderr, "%s: incomplete command\n", s);
        return;

    case UNRECOGNIZED:
        fprintf(stderr, "%s: unknown keyword `%s'\n", s, yylval.s);
        return;

    case NUM:
        fprintf(stderr, "%s: unexpected token `%u'\n", s, yylval.u);
        return;
    }

    if (isprint(yychar)) {
        fprintf(stderr, "%s: unexpected token `%c'\n", s, yychar);
    } else {
        fprintf(stderr, "%s: unexpected token `%s'\n", s, tokenstr(yychar));
    }
}

void range_error(unsigned a, unsigned b)
{
    fprintf(stderr, "syntax error: invalid range `%u-%u'\n", a, b);
}

int yywrap()
{
    return 1;
}
