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

%define lr.type ielr
%define parse.lac full

%{

#include <config.h>

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "commons.h"
#include "game.h"
#include "board.h"

void yyerror(const char *s);
int yywrap();
int yylex();

void year_error();
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
%token BOARD
%token BY
%token C
%token CLEAR
%token DELETE
%token LIST
%token H
%token OWNER
%token PHASE
%token RESET
%token RUN
%token S
%token SET
%token VIA
%token YEAR

%token <u> NATION
%token <i> TERR
%token <i> COAST
%token <i> UNIT
%token <i> ERA
%token <i> SEASON

%token <u> NUM

%token <s> UNRECOGNIZED

%type <i> era
%type <u> year

%type <tc> terr_coast
%type <tclist> tclist

%type <r> range
%type <rlist> range_list

%type <tlist> tlist

%type <b> viac

%start commands

%destructor { tclist_free($$); } <tclist>
%destructor { terrlist_free($$); } <tlist>
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
       | list
       | NATION { cur_nat = $1; }
       | BOARD  { print_board(); }
       | RESET  { board_init(); }
       | RUN    { adjudicate(); }

set: SET tclist UNIT NATION { set_terrs($2, $3, $4); tclist_free($2); }
   | SET OWNER tlist NATION { set_centers($3, $4); terrlist_free($3); }
   | SET YEAR year era      { year = ((int)$3) * $4; }
   | SET PHASE SEASON       { season = $3; }

clear: CLEAR tlist       { clear_terrs($2); terrlist_free($2); }
     | CLEAR OWNER tlist { clear_centers($3); terrlist_free($3); }
     | CLEAR ALL         { clear_all(); }

list: LIST NATION { list_orders($2); }
    | LIST ALL    { list_all_orders(); }
    | LIST        { list_orders(NO_NATION); }

tclist: terr_coast        { $$ = tclist_cons($1); }
      | tclist terr_coast { $$ = tclist_add($1, $2); }

terr_coast: TERR COAST { $$.terr = $1; $$.coast = $2; }
          | TERR       { $$.terr = $1; $$.coast = NO_COAST; }

year: NUM { if ($1 == 0) { year_error(); YYERROR; } }

era: ERA
   | /* Default */ { $$ = AD; }

delete: DELETE range_list { delete_orders($2); rangelist_free($2); }
      | DELETE ALL        { delete_all_orders(); }

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

tlist: TERR       { $$ = terrlist_cons($1); }
     | tlist TERR { $$ = terrlist_add($1, $2); }

viac: VIA C         { $$ = true; }
    | BY C          { $$ = true; }
    | C             { $$ = true; }
    | /* Nothing */ { $$ = false; }

order: tlist H                  { order_hold($1); terrlist_free($1); }
     | TERR '-' terr_coast viac { order_move($1, $3, $4); }
     | tlist S TERR             { order_suph($1, $3); terrlist_free($1); }
     | tlist S TERR '-' TERR    { order_supm($1, $3, $5); terrlist_free($1); }
     | tlist C TERR '-' TERR    { order_conv($1, $3, $5); terrlist_free($1); }

%%

const char *tokenstr(int token)
{
    static struct {
        int code;
        const char *name;
    } keywords[] = {
        {ALL,    "all"},
        {BOARD,  "board"},
        {BY,     "by"},
        {C,      "c"},
        {CLEAR,  "clear"},
        {DELETE, "delete"},
        {H,      "h"},
        {OWNER,  "owner"},
        {LIST,   "list"},
        {PHASE,  "phase"},
        {RESET,  "reset"},
        {RUN,    "run"},
        {S,      "s"},
        {SET,    "set"},
        {VIA,    "via"},
        {YEAR,   "year"},
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

    case SEASON:
        return get_season_name(yylval.i);

    case COAST:
    case UNIT:
    case ERA:
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

void year_error()
{
    fputs("syntax error: 0 is not a valid year\n", stderr);
}

void range_error(unsigned a, unsigned b)
{
    fprintf(stderr, "syntax error: invalid range `%u-%u'\n", a, b);
}

int yywrap()
{
    return 1;
}
