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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "commons.h"
#include "pprintf.h"
#include "game.h"

#define VALIDATE_CUR_NAT()             \
do {                                   \
    if (cur_nat == NO_NATION) {        \
        puts("Select a nation first"); \
        return;                        \
    }                                  \
} while (0)

int year = 1901;
enum season season = SPRING;

void print_date()
{
    enum era era = sgn(year);

    printf("%d %s - %s\n", abs(year),
                           get_era_name(era),
                           get_season_name(season));
}

enum cd_nation cur_nat = NO_NATION;

static size_t orders_n[NATIONS_N];
static struct order orders[NATIONS_N][TERR_N];

void reset_orders()
{
    cur_nat = NO_NATION;
    memset(orders_n, 0, sizeof orders_n);
}

size_t get_orders_base_index(enum cd_nation nat)
{
    size_t nat_i = trail0s(nat);
    size_t ret = 1;

    size_t n;
    for (n = 0; n < nat_i; n++) {
        ret += orders_n[n];
    }

    return ret;
}

size_t orders_n_tot()
{
    size_t ret = 0;

    enum cd_nation n;
    for (n = 0; n < NATIONS_N; n++) {
        ret += orders_n[n];
    }

    return ret;
}

void game_init()
{
    reset_orders();
    print_date();
}

int get_season(const char *name)
{
    if (istrcmp(name, "spring") == 0) {
        return SPRING;
    } else if (istrcmp(name, "autumn") == 0
               || istrcmp(name, "fall") == 0) {
        return AUTUMN;
    } else {
        return -1;
    }
}

const char *get_season_name(enum season season)
{
    switch (season) {
    case SPRING:
        return "Spring";

    case AUTUMN:
        return "Autumn";

    default:
        return "!INVALID SEASON!";
    }
}

const char *get_era_name(enum era era)
{
    switch (era) {
    case AD:
        return "AD";

    case BC:
        return "BC";

    default:
        return "!INVALID ERA!";
    }
}

int pprint_order(struct order o, bool newline)
{
    int ret;

    switch (o.kind) {
    case HOLD:
        ret = pprintf("%s H", get_terr_name(o.t1));
        break;

    case MOVE:
        ret = pprintf("%s-%s%s%s", get_terr_name(o.t2),
                                   get_terr_name(o.t3),
                                   get_coast_name(o.coast),
                                   o.viac ? " VIA C" : "");
        break;

    case SUPH:
        ret = pprintf("%s S %s", get_terr_name(o.t1),
                                 get_terr_name(o.t2));
        break;

    case SUPM:
        ret = pprintf("%s S %s-%s", get_terr_name(o.t1),
                                    get_terr_name(o.t2),
                                    get_terr_name(o.t3));
        break;

    case CONV:
        ret = pprintf("%s C %s-%s", get_terr_name(o.t1),
                                    get_terr_name(o.t2),
                                    get_terr_name(o.t3));
        break;

    default:
        ret = pprintf("!INVALID ORDER!");
        break;
    }

    if (newline) {
        pputchar('\n');
        return 0;
    } else {
        return ret;
    }
}

void delete_error(size_t i)
{
    printf("No such order: %zu\n", i);
}

void delete_orders(rangelist_t ranges)
{
    size_t i = 0, j;
    size_t size = 16;
    size_t *indices = malloc(size * sizeof *indices);

    rangelist_t l = ranges;
    while (l) {
        for (j = l->item.a; j < l->item.b; j++) {
            if (i >= size) {
                GROW_VEC(indices, size);
            }

            indices[i++] = j;
        }

        LIST_ADVANCE(l);
    }

    size_t len = i;

    qsort(indices, len, sizeof *indices, size_t_cmp);
    UNIQ(indices, len);

    if (indices[0] == 0) {
        delete_error(0);
        return;
    }

    size_t n = orders_n_tot();
    for (i = 0; i <= len; i++) {
        if (indices[i] > n) {
            delete_error(indices[i]);
            return;
        }
    }

    i = 0;
    j = 1;

    size_t k, o;
    for (n = 0; n < NATIONS_N; n++) {
        k = 0;
        for (o = 0; o < orders_n[n]; o++) {
            if (j == indices[i]) {
                i++;
            } else {
                orders[n][k++] = orders[n][o];
            }

            j++;
        }

        orders_n[n] = k;
    }

    free(indices);
}

void delete_all_orders()
{
    size_t n;
    for (n = 0; n < NATIONS_N; n++) {
        orders_n[n] = 0;
    }
}

void list_orders(enum cd_nation nat)
{
    pprintf_init();

    if (nat == NO_NATION) {
        VALIDATE_CUR_NAT();
        nat = cur_nat;
    }

    size_t nat_i = trail0s(nat);

    if (orders_n[nat_i] == 0) {
        pprintf("No orders from %s\n", get_nation_name(nat_i));
        return;
    }

    size_t base = get_orders_base_index(nat_i);
    int w = (int)decimal_places(base + orders_n[nat_i]);

    size_t i;
    for (i = 0; i < orders_n[nat_i]; i++) {
        pprintf("%*zu: ", w, base + i);
        pprint_order(orders[nat_i][i], true);
    }
}

void list_all_orders()
{
    pprintf_init();

    bool any = false;
    size_t i = 1;
    int w = (int)decimal_places(orders_n_tot());

    size_t n;
    for (n = 0; n < NATIONS_N; n++) {
        if (orders_n[n] == 0) {
            continue;
        }

        pputchar('\n');

        any = true;

        pprintf("%s\n", cd_nation_names[n]);

        size_t j;
        for (j = 0; j < orders_n[n]; j++) {
            pprintf("%*zu: ", w, i++);
            pprint_order(orders[n][j], true);
        }
    }

    if (!any) {
        pprintf("No orders\n");
    } else {
        pputchar('\n');
    }
}

void update_centers()
{
    /* TODO */
}

void update_units()
{
    /* TODO */
}

void advance_turn()
{
    if (season == SPRING) {
        season = AUTUMN;
    } else {
        year++;

        if (year == 0) {
            year = 1;
        }

        season = SPRING;
    }

    if (season == SPRING) {
        update_centers();
        update_units();
    }

    print_date();
}

bool dislodged(enum cd_terr t)
{
    size_t i;
    for (i = 0; i < cd_retreats_n; i++) {
        if (cd_retreats[i].who == t) {
            return true;
        }
    }

    return false;
}

struct move {
    enum cd_terr t1;
    enum cd_terr t2;
    enum cd_coast coast;
    enum cd_unit unit;
    enum cd_nation nation;
};

struct move successful_moves[TERR_N];
size_t successful_moves_n = 0;

void register_successful_move(struct order *o)
{
    size_t i;

    #ifndef NDEBUG
    for (i = 0; i < successful_moves_n; i++) {
        assert(successful_moves[i].t1 != o->t1);
    }
    #endif

    struct move *m = &successful_moves[successful_moves_n];

    m->t1     = o->t1;
    m->t2     = o->t3;
    m->coast  = o->coast;
    m->unit   = board[o->t1].unit;
    m->nation = board[o->t1].occupier;

    successful_moves_n++;
}

void execute_moves()
{
    size_t i;
    for (i = 0; i < successful_moves_n; i++) {
        struct move *m = &successful_moves[i];
        board[m->t1].occupier = NO_NATION;
        cd_clear_unit(m->t1);
    }

    for (i = 0; i < successful_moves_n; i++) {
        struct move *m = &successful_moves[i];
        board[m->t2].occupier = m->nation;
        board[m->t2].unit     = m->unit;
        board[m->t2].coast    = m->coast;
        cd_register_unit(m->t1, m->coast, m->unit, m->nation);
    }

    successful_moves_n = 0;
}

enum game_state {
    DEFAULT,
    RETREAT
};

enum game_state state = DEFAULT;

void adjudicate_orders()
{
    bool any = false;

    size_t nat_i, i;
    for (nat_i = 0; nat_i < NATIONS_N; nat_i++) {
        for (i = 0; i < orders_n[nat_i]; i++) {
            struct order *o = &orders[nat_i][i];

            any = true;

            if (board[o->t1].occupier != (1u << nat_i)) {
                continue;
            }

            switch (o->kind) {
            case MOVE:
                cd_register_move(o->t2, o->t3, o->coast, o->viac);
                break;

            case SUPH:
                cd_register_suph(o->t1, o->t2);
                break;

            case SUPM:
                cd_register_supm(o->t1, o->t2, o->t3);
                break;

            case CONV:
                cd_register_conv(o->t1, o->t2, o->t3);
                break;

            default:
                break;
            }
        }
    }

    cd_run_adjudicator();

    pprintf_init();
    pputchar('\n');

    size_t j = 0;

    for (nat_i = 0; nat_i < NATIONS_N; nat_i++) {
        if (orders_n[nat_i] == 0) {
            continue;
        }

        pprintf("%s\n", get_nation_name(1u << nat_i));

        for (i = 0; i < orders_n[nat_i]; i++) {
            struct order *o = &orders[nat_i][i];
            int w = pprint_order(*o, false);

            int i;
            for (i = 0; i < COL_WIDTH - w; i++) {
                pputchar(' ');
            }

            if (board[o->t1].occupier != (1u << nat_i)) {
                pprintf(" [IGNORED]\n");
                continue;
            }

            if (o->kind == HOLD) {
                if (dislodged(o->t1)) {
                    pprintf(" [FAILS]\n");
                } else {
                    pprintf(" [SUCCEEDS]\n");
                }

                continue;
            }

            if (cd_resolutions[j] == SUCCEEDS) {
                pprintf(" [SUCCEEDS]\n");

                if (o->kind == MOVE) {
                    register_successful_move(o);
                }
            } else {
                pprintf(" [FAILS]\n");
            }

            j++;
        }

        pputchar('\n');
    }

    if (!any) {
        printf("No orders\n\n");
    }

    reset_orders();

    if (cd_retreats_n > 0) {
        pprintf("Units dislodged:");

        for (i = 0; i < cd_retreats_n; i++) {
            pprintf(" %s", get_terr_name(cd_retreats[i].who));
        }

        pprintf("\nAwaiting retreats\n\n");
        state = RETREAT;
    } else {
        execute_moves();
        advance_turn();
    }
}

bool can_retreat(enum cd_terr t1,
                 enum cd_terr t2,
                 enum cd_coast coast)
{
    size_t i;
    for (i = 0; i < cd_retreats_n; i++) {
        if (cd_retreats[i].who != t1) {
            continue;
        }

        size_t j;
        for (j = 0; j < cd_retreats[i].where_n; j++) {
            if (cd_retreats[i].where[j].terr == t2
                && cd_retreats[i].where[j].coasts & coast) {

                return true;
            }
        }

        break;
    }

    return false;
}

void adjudicate_retreats()
{
    unsigned contenders[TERR_N];
    memset(contenders, 0, sizeof contenders);

    size_t nat_i, i;
    for (nat_i = 0; nat_i < NATIONS_N; nat_i++) {
        for (i = 0; i < orders_n[nat_i]; i++) {
            struct order *o = &orders[nat_i][i];

            if (board[o->t1].occupier != (1u << nat_i)
                || o->kind != MOVE) {
                continue;
            }

            if (can_retreat(o->t1, o->t3, o->coast)) {
                contenders[o->t3]++;
            }
        }
    }

    pprintf_init();
    pputchar('\n');

    bool any = false;

    for (nat_i = 0; nat_i < NATIONS_N; nat_i++) {
        if (orders_n[nat_i] == 0) {
            continue;
        }

        pprintf("%s\n", get_nation_name(1u << nat_i));

        for (i = 0; i < orders_n[nat_i]; i++) {
            any = true;

            struct order *o = &orders[nat_i][i];
            int w = pprint_order(*o, false);

            int i;
            for (i = 0; i < COL_WIDTH - w; i++) {
                pputchar(' ');
            }

            if (board[o->t1].occupier != (1u << nat_i)
                || !dislodged(o->t1)) {

                pprintf(" [IGNORED]\n");
                continue;
            }

            if (o->kind != MOVE
                || !can_retreat(o->t1, o->t3, o->coast)) {

                pprintf(" [FAILS]\n");
                continue;
            } else if (contenders[o->t3] > 1) {
                pprintf(" [FAILS] (bump)\n");
                continue;
            } else {
                pprintf(" [SUCCEEDS]\n");
                register_successful_move(o);
            }
        }

        putchar('\n');
    }

    if (!any) {
        pprintf("No retreat orders\n\n");
    }

    reset_orders();

    execute_moves();
    advance_turn();

    state = DEFAULT;
}

void adjudicate()
{
    switch (state) {
    case DEFAULT:
        adjudicate_orders();
        break;

    case RETREAT:
        adjudicate_retreats();
        break;

    default:
        break;
    }
}

size_t find_order(enum cd_nation nat, enum cd_terr terr)
{
    size_t nat_i = trail0s(nat);

    size_t i;
    for (i = 0; i < orders_n[nat_i]; i++) {
        if (orders[nat_i][i].t1 == terr) {
            break;
        }
    }

    return i;
}

void register_order(enum cd_nation nat,
                    enum order_kind kind,
                    enum cd_terr t1,
                    enum cd_terr t2,
                    enum cd_terr t3,
                    enum cd_coast coast,
                    bool viac)
{
    size_t nat_i = trail0s(nat);
    size_t i = find_order(nat, t1);

    orders[nat_i][i].kind  = kind;
    orders[nat_i][i].t1    = t1;
    orders[nat_i][i].t2    = t2;
    orders[nat_i][i].t3    = t3;
    orders[nat_i][i].coast = coast;
    orders[nat_i][i].viac  = viac;

    if (i >= orders_n[nat_i]) {
        orders_n[nat_i]++;
    }
}

void order_hold(terrlist_t tlist)
{
    VALIDATE_CUR_NAT();

    while (tlist) {
        register_order(cur_nat, HOLD, tlist->item,
                       NO_TERR, NO_TERR, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_move(enum cd_terr t2, struct terr_coast t3c, bool viac)
{
    VALIDATE_CUR_NAT();

    register_order(cur_nat, MOVE, t2, t2, t3c.terr, t3c.coast, viac);
}

void order_suph(terrlist_t tlist, enum cd_terr t2)
{
    VALIDATE_CUR_NAT();

    while (tlist) {
        register_order(cur_nat, SUPH, tlist->item, t2,
                       NO_TERR, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_supm(terrlist_t tlist, enum cd_terr t2, enum cd_terr t3)
{
    VALIDATE_CUR_NAT();

    while (tlist) {
        register_order(cur_nat, SUPM, tlist->item, t2, t3, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_conv(terrlist_t tlist, enum cd_terr t2, enum cd_terr t3)
{
    VALIDATE_CUR_NAT();

    while (tlist) {
        register_order(cur_nat, CONV, tlist->item, t2, t3, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}
