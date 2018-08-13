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

#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

#define DEFINE_LIST(name, type)                                         \
                                                                        \
struct name##list_cons {                                                \
    type car;                                                           \
    struct name##list_cons *cdr;                                        \
};                                                                      \
                                                                        \
typedef struct name##list_cons *name##list_t;                           \
                                                                        \
inline static name##list_t name##list_cons(type item)                   \
{                                                                       \
    name##list_t tmp = malloc(sizeof *tmp);                             \
    tmp->car = item;                                                    \
    tmp->cdr = NULL;                                                    \
    return tmp;                                                         \
}                                                                       \
                                                                        \
inline static name##list_t name##list_add(name##list_t list, type item) \
{                                                                       \
    name##list_t tmp = name##list_cons(item);                           \
    tmp->cdr = list;                                                    \
    return tmp;                                                         \
}                                                                       \
                                                                        \
inline static void name##list_free(name##list_t list)                   \
{                                                                       \
    while (list != NULL) {                                              \
        name##list_t cdr = list->cdr;                                   \
        free(list);                                                     \
        list = cdr;                                                     \
    }                                                                   \
}

#endif /* _LIST_H_ */
