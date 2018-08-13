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

#include "list.h"

enum nation {
    NO_NATION =  0,
    AUSTRIA   =  1,
    ENGLAND   =  2,
    FRANCE    =  4,
    GERMANY   =  8,
    ITALY     = 16,
    RUSSIA    = 32,
    TURKEY    = 64
};

enum unit {
    ARMY,
    FLEET
};

enum coast {
    NONE,
    NORTH,
    SOUTH
};

enum terr {
    NO_TERR = -1,
    ADR, AEG, ALB, ANK, APU, ARM,
    BAL, BAR, BEL, BER, BLA, BOH,
    BOT, BRE, BUD, BUL, BUR, CLY,
    CON, DEN, EAS, EDI, ENG, FIN,
    GAL, GAS, GRE, HEL, HOL, ION,
    IRI, KIE, LON, LVN, LVP, LYO,
    MAO, MAR, MOS, MUN, NAF, NAO,
    NAP, NTH, NWG, NWY, PAR, PIC,
    PIE, POR, PRU, ROM, RUH, RUM,
    SER, SEV, SIL, SKA, SMY, SPA,
    STP, SWE, SYR, TRI, TUN, TUS,
    TYR, TYS, UKR, VEN, VIE, WAL,
    WAR, WES, YOR,
    TERR_N
};

struct terr_coast {
    enum terr terr;
    enum coast coast;
};

DEFINE_LIST(tc, struct terr_coast);

struct terr {
    enum unit unit;
    enum coast coast;
    enum nation nation;
};

struct terr board[TERR_N];

void init_board();

void set_terrs(tclist_t tclist, enum unit unit, enum nation nation);

#endif /* _BOARD_H_ */
