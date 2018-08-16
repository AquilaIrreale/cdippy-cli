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

#include <stdbool.h>

#include "list.h"

#define ARRSIZE(a) (sizeof (a) / sizeof (a)[0])

#define SWAP(type, a, b) \
do {                     \
    type tmp = a;        \
    a = b;               \
    b = tmp;             \
} while (0)


struct range {
    unsigned a, b;
};

DEFINE_LIST(range, struct range);

bool strisblank(const char *s);
int istrcmp(const char *s1, const char *s2);
size_t decimal_places(size_t n);

#endif /* _COMMONS_H_ */
