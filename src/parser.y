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

#include "orders.h"
#include "game.h"

void yyerror(const char *s);
int yywrap();
int yylex();

%}

%union {
    int i;
    unsigned u;
    struct terr_coast tc;
    tclist_t tclist;
    struct range r;
    rangelist_t rlist;
}

%token SET
%token RESET
%token YEAR
%token PHASE 
%token DELETE

%token <i> NATION
%token <i> TERR
%token <i> COAST
%token <i> UNIT
%token <i> ERA
%token <i> SEASON

%token <u> NUM

%type <tc> terr_coast
%type <tclist> tlist

%type <r> range
%type <rlist> range_list

%start input

%%

input: /* Nothing */
     | input command '\n'

command: set
       | order
       | delete
       | clear
       | NATION { select_nation($1); }
       | LIST   { list_orders(); }
       | STATE  { print_state(); }
       | RESET  { load_defaults(); }
       | RUN    { adjudicate(); }

set: SET tclist UNIT NATION { set_terrs($2, $3, $4); tclist_free(&$2); }
   | SET YEAR NUM era       { set_year($3 * $4); }
   | SET PHASE SEASON       { set_phase($3); }

tclist: terr_coast        { $$ = tclist_cons($1); }
      | tclist terr_coast { $$ = tclist_add($1, $2); }

terr_coast: TERR COAST { $$.terr = $1; $$.coast = $2 }
          | TERR       { $$.terr = $1; $$.coast = NONE }

era: ERA
   | /* Default */ { $$ = AD }

delete: DELETE range_list { delete_orders($2); rangelist_free($2); }

range_list: range            { $$ = rangelist_cons($1); }
          | range_list range { $$ = rangelist_add($1, $2); }

range: NUM '-' NUM { $$ = {$1, $3 + 1} }
     | NUM         { $$ = {$1, $1 + 1} }

%%

void yyerror(const char *s)
{
    fputs(s, stderr);
    fputc('\n', stderr);
    exit(1);
}

int yywrap()
{
    return 1;
}
