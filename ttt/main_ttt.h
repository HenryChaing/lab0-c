#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "mcts.h"
#include "negamax.h"

void record_move(int move);
void print_moves();
int get_input(char player);
void main_ttt(int mode);