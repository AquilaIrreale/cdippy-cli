AM_CFLAGS = -Wall -Wextra
AM_YFLAGS = -d

BUILT_SOURCES = src/parser.h
noinst_HEADERS = src/parser.h

bin_PROGRAMS = cdippy-cli
cdippy_cli_SOURCES = src/board.c \
                     src/game.c \
                     src/lexer.l \
                     src/main.c \
                     src/parser.y \

CLEANFILES = src/parser.h \
             src/parser.c \
             src/lexer.c

EXTRA_DIST = README.md #testsuite
#RUNTESTDEFAULTFLAGS = --all --tool cdippy CDIPPY-CLI=./cdippy-cli \
#                      --srcdir $$srcdir/testsuite
