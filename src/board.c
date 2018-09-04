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
#include <string.h>
#include <assert.h>

#include "board.h"
#include "commons.h"
#include "pprintf.h"

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

const char *get_unit_name(enum cd_unit unit)
{
    if (unit == ARMY) {
        return "A";
    } else {
        return "F";
    }
}

void print_board()
{
    pprintf_init();

    enum cd_terr t;
    for (t = 0; t < TERR_N; t++) {
        struct terr_info *ti = &board[t];
        if (!ti->supp_center && !ti->occupier) {
            continue;
        }

        char buf[20];

        if (ti->occupier == NO_NATION) {
            sprintf(buf, "%s: Not occupied", get_terr_name(t));
        } else {
            sprintf(buf, "%s: %s%s %s", get_terr_name(t),
                                        get_unit_name(ti->unit),
                                        get_coast_name(ti->coast),
                                        get_nation_name(ti->occupier));
        }

        pprintf("%*s", -COL_WIDTH, buf);

        if (ti->supp_center) {
            pprintf(" (%s)", ti->owner != NO_NATION
                           ? get_nation_name(ti->owner)
                           : "independent");
        }

        pputchar('\n');
    }
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

        cd_clear_unit(t);
    }

    size_t i;
    for (i = 0; i < ARRSIZE(nations); i++) {
        size_t j;
        for (j = 0; nations[i].terrs[j] != NO_TERR; j++) {
            cd_register_unit(nations[i].terrs[j],
                             nations[i].terrs[j] != STP ? NO_COAST : SOUTH,
                             nations[i].units[j],
                             nations[i].nat);

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
    pprintf_init();

    for (; tclist != NULL; LIST_ADVANCE(tclist)) {
        enum cd_terr t = tclist->item.terr;
        enum cd_coast coast = tclist->item.coast;

        int err = cd_register_unit(t, coast, unit, nation);
        if (err) {
            assert(err != CD_INVALID_TERR);

            const char *errors[] = {
                NULL,
                NULL,
                "coast specified for single-coast territory",
                "you have to specify a coast",
                "coast specified but unit is an army",
                "cannot place an army in sea",
                "cannot place a fleet on land"
            };

            pprintf("%s%s: %s\n",
                    get_terr_name(t),
                    get_coast_name(coast),
                    errors[err]);

            continue;
        }

        board[t].occupier = nation;
        board[t].unit = unit;
        board[t].coast = coast;
    }
}

void set_centers(terrlist_t tlist, enum cd_nation nation)
{
    pprintf_init();

    while (tlist != NULL) {
        if (board[tlist->item].supp_center) {
            board[tlist->item].owner = nation;
        } else {
            pprintf("%s: not a supply center\n", tlist->item);
        }

        LIST_ADVANCE(tlist);
    }
}

void clear_terrs(terrlist_t tlist)
{
    while (tlist != NULL) {
        board[tlist->item].occupier = NO_NATION;
        cd_clear_unit(tlist->item);
        LIST_ADVANCE(tlist);
    }
}

void clear_centers(terrlist_t tlist)
{
    pprintf_init();

    while (tlist != NULL) {
        if (board[tlist->item].supp_center) {
            board[tlist->item].owner = NO_NATION;
        } else {
            pprintf("%s: not a supply center\n", tlist->item);
        }

        LIST_ADVANCE(tlist);
    }
}

void clear_all()
{
    enum cd_terr t;
    for (t = 0; t < TERR_N; t++) {
        board[t].occupier = NO_NATION;
        board[t].owner = NO_NATION;

        cd_clear_unit(t);
    }
}
