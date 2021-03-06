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

#include <readline/readline.h>

#include "commons.h"
#include "list.h"
#include "pprintf.h"
#include "board.h"
#include "game.h"

#define VALIDATE_CUR_NAT()             \
do {                                   \
    if (cur_nat == NO_NATION) {        \
        puts("Select a nation first"); \
        return;                        \
    }                                  \
} while (0)

#define VALIDATE_STATE_NOT(...)                       \
do {                                                  \
    static const enum game_state p[] = {__VA_ARGS__}; \
    size_t i;                                         \
    for (i = 0; i < ARRSIZE(p); i++) {                \
        if (state == p[i]) {                          \
            printf("Cannot do that now (%s)\n",       \
                   state_names[state]);               \
            return;                                   \
        }                                             \
    }                                                 \
} while (0);

static const char *state_names[] = {
    "main phase",
    "retreat phase",
    "build phase"
};

enum game_state {
    DEFAULT,
    RETREAT,
    BUILD
};

enum game_state state = DEFAULT;

void set_state(enum game_state new_state)
{
    state = new_state;

    switch (state) {
    case DEFAULT:
        printf("Awaiting orders\n\n");
        break;

    case RETREAT:
        printf("Awaiting retreats\n\n");
        break;

    case BUILD:
        printf("Awaiting build orders\n\n");
        break;

    default:
        break;
    }
}

int year = 1901;
enum season season = SPRING;

void print_date()
{
    enum era era = sgn(year);

    printf("== %d %s - %s ==\n", abs(year),
                                 get_era_name(era),
                                 get_season_name(season));
}

enum cd_nation cur_nat = NO_NATION;

static struct order orders[NATIONS_N][TERR_N];
static size_t orders_n[NATIONS_N];

static unsigned to_build[NATIONS_N];

void print_build_digest()
{
    puts("Some nations can build new units:");

    size_t i;
    for (i = 0; i < NATIONS_N; i++) {
        enum cd_nation nat = 1u << i;

        if (to_build[i] > 0) {
            printf("%-7s %2u\n",
                   get_nation_name(nat),
                   to_build[i]);
        }
    }

    putchar('\n');
}

void reset_orders()
{
    cur_nat = NO_NATION;
    memset(orders_n, 0, sizeof orders_n);
}

void game_init()
{
    memset(to_build, 0, sizeof to_build);
    reset_orders();
    print_date();
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

int pprint_order(struct order *o)
{
    switch (o->kind) {
    case HOLD:
        return pprintf("%s H", get_terr_name(o->t1));

    case MOVE:
        return pprintf("%s-%s%s%s", get_terr_name(o->t2),
                                    get_terr_name(o->t3),
                                    get_coast_name(o->coast),
                                    o->viac ? " VIA C" : "");

    case SUPH:
        return pprintf("%s S %s", get_terr_name(o->t1),
                                  get_terr_name(o->t2));

    case SUPM:
        return pprintf("%s S %s-%s", get_terr_name(o->t1),
                                     get_terr_name(o->t2),
                                     get_terr_name(o->t3));

    case CONV:
        return pprintf("%s C %s-%s", get_terr_name(o->t1),
                                     get_terr_name(o->t2),
                                     get_terr_name(o->t3));

    default:
        return pprintf("!INVALID ORDER!");
    }
}

int pprint_build_order(struct order *o)
{
    return pprintf("%s %s%s",
                   get_unit_name(o->unit),
                   get_terr_name(o->t1),
                   get_coast_name(o->coast));
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

/* TODO: list build orders in build phase */
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
        pprint_order(&orders[nat_i][i]);
        pputchar('\n');
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
            pprint_order(&orders[n][j]);
            pputchar('\n');
        }
    }

    if (!any) {
        pprintf("No orders\n");
    } else {
        pputchar('\n');
    }
}

char *parse_tlist(char *s, terrlist_t *ret)
{
    assert(s != NULL);

    char *c;
    for (c = s; *c != '\0'; c++) {
        if (!isalpha(*c) && *c != ' ' && *c != '\t') {
            c[1] = '\0';
            return c;
        }
    }

    char *tok = strtok(s, " \t");

    if (tok == NULL) {
        *ret = NULL;
        return NULL;
    }

    terrlist_t tlist = NULL;

    do {
        enum cd_terr t = get_terr(tok);
        if (t == NO_TERR) {
            terrlist_free(tlist);
            return tok;
        }

        tlist = terrlist_add(tlist, t);
    } while ((tok = strtok(NULL, " \t")));

    *ret = tlist;
    return NULL;
}

void remove_units(enum cd_nation nat, unsigned n)
{
    printf("%s has too many units, choose %u to be disbanded (units:",
           get_nation_name(nat), n);

    enum cd_terr t;
    for (t = 0; t < TERR_N; t++) {
        if (board[t].occupier == nat) {
            printf(" %s", get_terr_name(t));
        }
    }

    puts(")");

    for (;;) {
        char *line = readline("D" PROMPT);

        terrlist_t tlist;
        char *ret = parse_tlist(line, &tlist);
        free(line);
        if (ret) {
            printf("syntax error: invalid character or token `%s'", ret);
            continue;
        }

        enum cd_terr invalid = NO_TERR;
        size_t count = 0;

        terrlist_t it;
        for (it = tlist; it != NULL; LIST_ADVANCE(it)) {
            if (board[it->item].occupier != nat) {
                invalid = it->item;
            }

            count++;
        }

        if (invalid != NO_TERR) {
            printf("Cannot disband %s\n", get_terr_name(invalid));
            continue;
        }

        if (count != n) {
            printf("Must disband exactly %u units\n", n);
            continue;
        }

        clear_terrs(tlist);
        terrlist_free(tlist);

        break;
    }
}

bool update_units()
{
    count_units();
    count_centers();

    bool build = false;

    size_t i;
    for (i = 0; i < NATIONS_N; i++) {
        enum cd_nation nat = 1u << i;

        to_build[i] = 0;

        if (centers[i] == 0) {
            printf("%s lost\n", get_nation_name(nat));
            remove_all_units(nat);
        } else if (units[i] > centers[i]) {
            remove_units(nat, units[i] - centers[i]);
        } else if (units[i] < centers[i]) {
            unsigned delta = centers[i] - units[i];
            unsigned avail = available_home_centers(nat);
            unsigned min = delta < avail ? delta : avail;

            if (min > 0) {
                to_build[i] = min;
                build = true;
            }
        }
    }

    return build;
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

    print_date();

    bool build = false;

    if (season == SPRING) {
        update_centers();
        build = update_units();
    }

    if (build) {
        print_build_digest();
        set_state(BUILD);
    } else {
        set_state(DEFAULT);
    }
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
            int w = pprint_order(o);

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

        pputchar('\n');

        set_state(RETREAT);
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
            int w = pprint_order(o);

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
}

void execute_build_orders()
{
    putchar('\n');

    pprintf_init();

    if (orders_n_tot() == 0) {
        pprintf("No build orders\n\n");
    }

    size_t i;
    for (i = 0; i < NATIONS_N; i++) {
        enum cd_nation nat = 1 << i;

        if (orders_n[i] == 0) {
            continue;
        }

        pprintf("%s\n", get_nation_name(nat));

        size_t j;
        for (j = 0; j < orders_n[i]; j++) {
            struct order *o = &orders[i][j];

            pprint_build_order(o);

            if (!is_home_center(o->t1, nat)) {
                pprintf(" [FAILS] (not a home center)\n");
                continue;
            }

            if (board[o->t1].owner != nat) {
                pprintf(" [FAILS] (center is under enemy control)\n");
                continue;
            }

            if (board[o->t1].occupier != NO_NATION) {
                pprintf(" [FAILS] (center is occupied)\n");
                continue;
            }

            int err = cd_register_unit(o->t1, o->coast, o->unit, nat);
            if (err) {
                assert(err != CD_INVALID_TERR);
                assert(err != CD_ARMY_IN_SEA);

                static const char *errors[] = {
                    NULL,
                    NULL,
                    "only has one coast",
                    "coast not unspecified",
                    "coast specified for army",
                    NULL,
                    "building fleet on land"
                };

                pprintf(" [FAILS] (%s)\n", errors[err]);
                continue;
            }

            board[o->t1].occupier = nat;
            board[o->t1].coast    = o->coast;
            board[o->t1].unit     = o->unit;

            pprintf(" [SUCCEEDS]\n");
        }

        pputchar('\n');
    }

    reset_orders();

    set_state(DEFAULT);
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

    case BUILD:
        execute_build_orders();
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
    VALIDATE_STATE_NOT(BUILD);
    VALIDATE_CUR_NAT();

    while (tlist) {
        register_order(cur_nat, HOLD, tlist->item,
                       NO_TERR, NO_TERR, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_move(enum cd_terr t2, struct terr_coast t3c, bool viac)
{
    VALIDATE_STATE_NOT(BUILD);
    VALIDATE_CUR_NAT();

    register_order(cur_nat, MOVE, t2, t2, t3c.terr, t3c.coast, viac);
}

void order_suph(terrlist_t tlist, enum cd_terr t2)
{
    VALIDATE_STATE_NOT(BUILD);
    VALIDATE_CUR_NAT();

    while (tlist) {
        register_order(cur_nat, SUPH, tlist->item, t2,
                       NO_TERR, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_supm(terrlist_t tlist, enum cd_terr t2, enum cd_terr t3)
{
    VALIDATE_STATE_NOT(BUILD);
    VALIDATE_CUR_NAT();

    while (tlist) {
        register_order(cur_nat, SUPM, tlist->item, t2, t3, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_conv(terrlist_t tlist, enum cd_terr t2, enum cd_terr t3)
{
    VALIDATE_STATE_NOT(BUILD);
    VALIDATE_CUR_NAT();

    while (tlist) {
        register_order(cur_nat, CONV, tlist->item, t2, t3, NO_COAST, false);
        LIST_ADVANCE(tlist);
    }
}

void order_build(tclist_t tclist, enum cd_unit unit)
{
    VALIDATE_STATE_NOT(DEFAULT, RETREAT);
    VALIDATE_CUR_NAT();

    size_t nat_i = trail0s(cur_nat);

    size_t c = 0;
    tclist_t it;
    for (it = tclist; it != NULL; LIST_ADVANCE(it)) {
        size_t i = find_order(cur_nat, tclist->item.terr);

        if (i >= orders_n[nat_i]) {
            c++;
        }
    }

    if (orders_n[nat_i] + c > to_build[nat_i]) {
        printf("Can only build up to %u new units\n", to_build[nat_i]);
        return;
    }

    while (tclist) {
        size_t i = find_order(cur_nat, tclist->item.terr);

        orders[nat_i][i].t1    = tclist->item.terr;
        orders[nat_i][i].coast = tclist->item.coast;
        orders[nat_i][i].unit  = unit;

        if (i >= orders_n[nat_i]) {
            orders_n[nat_i]++;
        }

        LIST_ADVANCE(tclist);
    }
}
