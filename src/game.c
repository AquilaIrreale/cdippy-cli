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

#include <stdio.h>

#include "game.h"

static enum nation cur_nat = NO_NATION;

static size_t orders_n[NATIONS_N];
static struct order orders[NATIONS_N][TERR_N];

void game_init()
{
    phase_init();
}

void phase_init()
{
    size_t i;
    for (i = 0; i < NATIONS_N; i++) {
        orders_n[i] = 0;
    }
}

void set_phase()
{
    /* TODO */
}

void set_year()
{
    /* TODO */
}

void select_nation(enum nation nation)
{
    cur_nat = nation;
}

void print_order(struct order o, bool newline)
{
    switch (o.kind) {
    case HOLD:
        printf("%s H", get_terr_name(o.t1));
        break;

    case MOVE:
        printf("%s-%s%s%s", get_terr_name(o.t2),
                            get_terr_name(o.t3),
                            get_coast_name(o.coast),
                            o.viac ? " VIA C" : "");
        break;

    case SUPH:
        printf("%s S %s", get_terr_name(o.t1),
                          get_terr_name(o.t2));
        break;

    case SUPM:
        printf("%s S %s-%s", get_terr_name(o.t1),
                             get_terr_name(o.t2),
                             get_terr_name(o.t3));
        break;

    case CONV:
        printf("%s C %s-%s", get_terr_name(o.t1),
                             get_terr_name(o.t2),
                             get_terr_name(o.t3));
        break;

    default:
        fputs("!INVALID ORDER!", stdout);
        break;
    }

    if (newline) {
        putchar('\n');
    }
}

void delete_orders(rangelist_t list)
{
    while (list != NULL) {
        if (list->item.a >= list->item.b) {
            printf("Invalid range: %u-%u\n", list->item.a, list->item.b - 1);
        } else if (list->item.a == list->item.b - 1) {
            printf("%u\n", list->item.a);
        } else {
            printf("%u-%u\n", list->item.a, list->item.b - 1);
        }

        LIST_ADVANCE(list);
    }
}

void list_orders()
{
    /* TODO */
}

void adjudicate()
{
    /* TODO */
}

size_t find_order(enum nation nat, enum terr terr)
{
    size_t i;
    for (i = 0; i < orders_n[nat]; i++) {
        if (orders[nat][i].t1 == terr) {
            break;
        }
    }

    return i;
}

void register_order(enum nation nat,
                    enum order_kind kind,
                    enum terr t1,
                    enum terr t2,
                    enum terr t3,
                    enum coast coast,
                    bool viac)
{
    size_t i = find_order(nat, t1);
    orders[nat][i].kind  = kind;
    orders[nat][i].t1    = t1;
    orders[nat][i].t2    = t2;
    orders[nat][i].t3    = t3;
    orders[nat][i].coast = coast;
    orders[nat][i].viac  = viac;
}

void order_hold(terrlist_t tlist)
{
    while (tlist) {
        register_order(cur_nat, HOLD, tlist->item,
                       NO_TERR, NO_TERR, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_move(enum terr t2, struct terr_coast t3c, bool viac)
{
    register_order(cur_nat, MOVE, t2, t2, t3c.terr, t3c.coast, viac);
}

void order_suph(terrlist_t tlist, enum terr t2)
{
    while (tlist) {
        register_order(cur_nat, SUPH, tlist->item, t2,
                       NO_TERR, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_supm(terrlist_t tlist, enum terr t2, enum terr t3)
{
    while (tlist) {
        register_order(cur_nat, SUPM, tlist->item, t2, t3, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_conv(terrlist_t tlist, enum terr t2, enum terr t3)
{
    while (tlist) {
        register_order(cur_nat, CONV, tlist->item, t2, t3, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}
