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
#include <errno.h>

#include <readline/history.h>

#include "commons.h"
#include "board.h"
#include "game.h"

#include "parser.h"

#define HIST_FILE ".cdippy-cli_history"

char *hist_path;

void save_history()
{
    int ret = write_history(hist_path);
    if (ret != 0) {
        perror("save_history");
    }
}

void load_history()
{
    char *home = getenv("HOME");

    if (!home) {
        return;
    }

    hist_path = malloc(strlen(home) + strlen(HIST_FILE) + 2);
    sprintf(hist_path, "%s/%s", home, HIST_FILE);

    int ret = read_history(hist_path);
    if (ret != 0 && ret != ENOENT) {
        perror("load_history");
    }
}

int main()
{
    using_history();
    load_history();
    atexit(save_history);

    board_init();
    game_init();

    yyparse();

    return 0;
}
