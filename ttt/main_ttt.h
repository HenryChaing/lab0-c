#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../list.h"
#include "game.h"
#include "mcts.h"
#include "me.h"
#include "negamax.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void print_all_moves();
void record_move(int move);
void print_moves();
int get_input(char player);
void main_ttt(int mode);