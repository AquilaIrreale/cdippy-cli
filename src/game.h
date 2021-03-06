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

#ifndef _GAME_H_
#define _GAME_H_

#include <cdippy.h>

#include "commons.h"
#include "board.h"

#define GOAL 18

extern enum cd_nation cur_nat;

enum era {
    BC = -1,
    AD = 1
};

enum season {
    SPRING,
    AUTUMN
};

extern int year;
extern enum season season;

int get_season(const char *name);

const char *get_era_name(enum era era);
const char *get_season_name(enum season season);

enum order_kind {
    NULL_ORDER,
    HOLD,
    MOVE,
    SUPH,
    SUPM,
    CONV
};

struct order {
    enum order_kind kind;
    enum cd_terr t1;
    enum cd_terr t2;

    union {
        enum cd_terr t3;
        enum cd_unit unit;
    };

    enum cd_coast coast;
    bool viac;
};

void game_init();
void phase_init();

void set_year();
void select_nation();

void order_hold(terrlist_t tlist);
void order_move(enum cd_terr t2, struct terr_coast t3c, bool viac);
void order_suph(terrlist_t tlist, enum cd_terr t2);
void order_supm(terrlist_t tlist, enum cd_terr t2, enum cd_terr t3);
void order_conv(terrlist_t tlist, enum cd_terr t2, enum cd_terr t3);

void order_build(tclist_t tclist, enum cd_unit unit);

void delete_orders(rangelist_t ranges);
void delete_all_orders();
void list_orders(enum cd_nation nat);
void list_all_orders();
void adjudicate();

#endif /* _GAME_H_ */
