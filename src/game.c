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

void set_phase()
{
    /* TODO */
}

void set_year()
{
    /* TODO */
}

void select_nation()
{
    /* TODO */
}

void delete_orders(rangelist_t list)
{
    while (list != NULL) {
        if (list->car.a >= list->car.b) {
            printf("Invalid range: %u-%u\n", list->car.a, list->car.b - 1);
        } else if (list->car.a == list->car.b - 1) {
            printf("%u\n", list->car.a);
        } else {
            printf("%u-%u\n", list->car.a, list->car.b - 1);
        }

        list = list->cdr;
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

void order_hold(terrlist_t tlist)
{
    /* TODO */
}

void order_move(enum terr t2, struct terr_coast t3c, bool viac)
{
    /* TODO */
}

void order_suph(terrlist_t tlist, enum terr t2)
{
    /* TODO */
}

void order_supm(terrlist_t tlist, enum terr t2, enum terr t3)
{
    /* TODO */
}

void order_conv(terrlist_t tlist, enum terr t2, enum terr t3)
{
    /* TODO */
}
