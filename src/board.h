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

#ifndef _BOARD_H_
#define _BOARD_H_

#include <cdippy.h>

#include "list.h"

struct terr_coast {
    enum cd_terr terr;
    enum cd_coast coast;
};

DEFINE_LIST(terr, enum cd_terr);
DEFINE_LIST(tc, struct terr_coast);

struct terr_info {
    enum cd_unit unit;
    enum cd_coast coast;
    enum cd_nation nation;
};

struct terr_info board[TERR_N];

void print_board();

int get_terr(const char *name);
unsigned get_nation(const char *name);

const char *get_terr_name(enum cd_terr terr);
const char *get_coast_name(enum cd_coast coast);
const char *get_nation_name(enum cd_nation nation);

void board_init();
void set_terrs(tclist_t tclist, enum cd_unit unit, enum cd_nation nation);
void clear_terrs(tclist_t tclist);
void clear_all_terrs();

#endif /* _BOARD_H_ */
