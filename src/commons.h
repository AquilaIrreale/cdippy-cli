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

#ifndef _COMMONS_H_
#define _COMMONS_H_

#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#include "list.h"

#define PROMPT "> "
#define COL_WIDTH 18

#define ARRSIZE(a) (sizeof (a) / sizeof (a)[0])

#define IS_POW2(n) ((n) > 0 && !((n) & ((n)-1)))

static inline size_t trail0s(size_t n)
{
    if (n == 0) {
        return CHAR_BIT * sizeof(n);
    }

    n = (n ^ (n-1)) >> 1;

    size_t c;
    for (c = 0; n > 0; c++) {
        n >>= 1;
    }

    return c;
}

static inline int sgn(int n)
{
    return n > 0 ?  1
         : n < 0 ? -1
         : 0;
}

#define SWAP(type, a, b) \
do {                     \
    type _tmp = a;       \
    a = b;               \
    b = _tmp;            \
} while (0)

#define GROW_VEC(v, s)             \
do {                               \
    s *= 3;                        \
    s /= 2;                        \
    v = realloc(v, s * sizeof *v); \
} while (0)

#define UNIQ(a, n)                    \
do {                                  \
    size_t _i, _j;                    \
    for (_i = _j = 0; _j < n; _j++) { \
        if (a[_j] != a[_i]) {         \
            a[++_i] = a[_j];          \
        }                             \
    }                                 \
    n = _i + 1;                       \
} while (0)

struct range {
    unsigned a, b;
};

DEFINE_LIST(range, struct range);

bool strisblank(const char *s);
int istrcmp(const char *s1, const char *s2);
size_t decimal_places(size_t n);
int size_t_cmp(const void *x, const void *y);

#endif /* _COMMONS_H_ */
