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

static const char *terr_names[] = {
    "ADR", "AEG", "Alb", "Ank", "Apu", "Arm",
    "BAL", "BAR", "Bel", "Ber", "BLA", "Boh",
    "BOT", "Bre", "Bud", "Bul", "Bur", "Cly",
    "Con", "Den", "EAS", "Edi", "ENG", "Fin",
    "Gal", "Gas", "Gre", "HEL", "Hol", "ION",
    "IRI", "Kie", "Lon", "Lvn", "Lvp", "LYO",
    "MAO", "Mar", "Mos", "Mun", "NAf", "NAO",
    "Nap", "NTH", "NWG", "Nwy", "Par", "Pic",
    "Pie", "Por", "Pru", "Rom", "Ruh", "Rum",
    "Ser", "Sev", "Sil", "SKA", "Smy", "Spa",
    "StP", "Swe", "Syr", "Tri", "Tun", "Tus",
    "Tyr", "TYS", "Ukr", "Ven", "Vie", "Wal",
    "War", "WES", "Yor"
};

static const char *nation_names[] = {
    "AUSTRIA",
    "ENGLAND",
    "FRANCE",
    "GERMANY",
    "ITALY",
    "RUSSIA",
    "TURKEY"
};

int istrcmp_wrapper(const void *a, const void *b)
{
    return istrcmp(a, *(char **)b);
}

int get_terr(const char *name)
{
    const char **found = bsearch(name, terr_names, ARRSIZE(terr_names),
                                 sizeof terr_names[0], istrcmp_wrapper);

    if (!found) {
        return NO_TERR;
    }

    return found - &terr_names[0];
}

int get_nation(const char *name)
{
    const char **found = bsearch(name, nation_names, ARRSIZE(nation_names),
                                 sizeof nation_names[0], istrcmp_wrapper);

    if (!found) {
        return NO_NATION;
    }

    return found - &nation_names[0];
}

const char *get_terr_name(enum terr terr)
{
    if (terr < 0 || terr >= TERR_N) {
        return "!INVALID TERR!";
    }

    return terr_names[terr];
}

const char *get_nation_name(enum nation nation)
{
    if (nation < 0 || nation >= NATIONS_N) {
        return "!INVALID NATION!";
    }

    return nation_names[nation];
}

const char *get_coast_name(enum coast coast)
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
    /* TODO */
}

void set_terrs(tclist_t tclist, enum unit unit, enum nation nation)
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
