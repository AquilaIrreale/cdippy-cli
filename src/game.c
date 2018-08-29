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

#define VALIDATE_CUR_NAT()             \
do {                                   \
    if (cur_nat == NO_NATION) {        \
        puts("Select a nation first"); \
        return;                        \
    }                                  \
} while (0)

static enum cd_nation cur_nat = NO_NATION;

static size_t orders_n[NATIONS_N];
static struct order orders[NATIONS_N][TERR_N];

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

void select_nation(enum cd_nation nation)
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
    if (nat == NO_NATION) {
        VALIDATE_CUR_NAT();
        nat = cur_nat;
    }

    size_t nat_i = trail0s(nat);

    if (orders_n[nat_i] == 0) {
        printf("No orders from %s\n", get_nation_name(nat_i));
        return;
    }

    size_t base = get_orders_base_index(nat_i);
    int w = (int)decimal_places(base + orders_n[nat_i]);

    size_t i;
    for (i = 0; i < orders_n[nat_i]; i++) {
        printf("%*zu: ", w, base + i);
        print_order(orders[nat_i][i], true);
    }
}

void list_all_orders()
{
    bool any = false;
    size_t i = 1;
    int w = (int)decimal_places(orders_n_tot());

    enum cd_nation n;
    for (n = 0; n < NATIONS_N; n++) {
        if (orders_n[n] == 0) {
            continue;
        }

        putchar('\n');

        any = true;

        puts(get_nation_name(n));

        size_t j;
        for (j = 0; j < orders_n[n]; j++) {
            printf("%*zu: ", w, i++);
            print_order(orders[n][j], true);
        }
    }

    if (!any) {
        puts("No orders");
    } else {
        putchar('\n');
    }
}

void adjudicate()
{
    /* TODO */
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
