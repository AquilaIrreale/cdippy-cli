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

#include "board.h"
#include "commons.h"

struct terr_info board[TERR_N];

int istrcmp_wrapper(const void *a, const void *b)
{
    return istrcmp(a, *(char **)b);
}

int get_terr(const char *name)
{
    const char **found = bsearch(name, cd_terr_names, TERR_N,
                                 sizeof cd_terr_names[0], istrcmp_wrapper);

    if (!found) {
        return NO_TERR;
    }

    return found - &cd_terr_names[0];
}

unsigned get_nation(const char *name)
{
    const char **found = bsearch(name, cd_nation_names, NATIONS_N,
                                 sizeof cd_nation_names[0], istrcmp_wrapper);

    if (!found) {
        return NO_NATION;
    }

    return 1 << (found - &cd_nation_names[0]);
}

const char *get_terr_name(enum cd_terr terr)
{
    if (terr < 0 || terr >= TERR_N) {
        return "!INVALID TERR!";
    }

    return cd_terr_names[terr];
}

const char *get_nation_name(enum cd_nation nation)
{
    size_t i;
    if (!IS_POW2(nation) || (i = trail0s(nation)) >= NATIONS_N) {
        return "!INVALID NATION!";
    }

    return cd_nation_names[i];
}

const char *get_coast_name(enum cd_coast coast)
{
    switch (coast) {
    case NORTH:
        return "(NC)";
    case SOUTH:
        return "(SC)";
    case NO_COAST:
        return "";
    default:
        return "!INVALID COAST!";
    }
}

void print_board()
{
    /* TODO */
}

void board_init()
{
    enum cd_terr centers[] = {
        ANK, BEL, BER, BRE, BUD, BUL, CON,
        DEN, EDI, GRE, HOL, KIE, LON, LVP,
        MAR, MOS, MUN, NAP, NWY, PAR, POR,
        ROM, RUM, SER, SEV, SMY, SPA, STP,
        SWE, TRI, TUN, VEN, VIE, WAR
    };

    size_t i;
    for (i = 0; i < ARRSIZE(centers); i++) {
        board[centers[i]].supp_center = true;
    }

    board_reset();
}

void board_reset()
{
    struct {
        enum cd_nation nat;
        enum cd_terr terrs[5];
        enum cd_unit units[4];
    } nations[] = {
        {
            AUSTRIA,
            {BUD, TRI, VIE, NO_TERR},
            {ARMY, FLEET, ARMY}
        }, {
            ENGLAND,
            {EDI, LON, LVP, NO_TERR},
            {FLEET, FLEET, ARMY}
        }, {
            FRANCE,
            {BRE, MAR, PAR, NO_TERR},
            {FLEET, ARMY, ARMY}
        }, {
            GERMANY,
            {BER, MUN, KIE, NO_TERR},
            {ARMY, ARMY, FLEET}
        }, {
            ITALY,
            {NAP, ROM, VEN, NO_TERR},
            {FLEET, ARMY, ARMY}
        }, {
            RUSSIA,
            {MOS, SEV, STP, WAR, NO_TERR},
            {ARMY, FLEET, FLEET, ARMY}
        }, {
            TURKEY,
            {ANK, CON, SMY, NO_TERR},
            {FLEET, ARMY, ARMY}
        }
    };

    enum cd_terr t;
    for (t = 0; t < TERR_N; t++) {
        board[t].occupier = NO_NATION;
        board[t].owner = NO_NATION;
    }

    size_t i;
    for (i = 0; i < ARRSIZE(nations); i++) {
        size_t j;
        for (j = 0; nations[i].terrs[j] != NO_TERR; j++) {
            board[nations[i].terrs[j]].occupier = nations[i].nat;
            board[nations[i].terrs[j]].owner = nations[i].nat;
            board[nations[i].terrs[j]].unit = nations[i].units[j];
            board[nations[i].terrs[j]].coast = NO_COAST;
        }
    }

    board[STP].coast = SOUTH;
}

void set_terrs(tclist_t tclist, enum cd_unit unit, enum cd_nation nation)
{
    while (tclist != NULL) {
        printf("%s %s\n", get_terr_name(tclist->item.terr), get_coast_name(tclist->item.coast));
        LIST_ADVANCE(tclist);
    }
}

void clear_terrs(tclist_t tclist)
{
    /* TODO */
}

void clear_all_terrs()
{
    /* TODO */
}
