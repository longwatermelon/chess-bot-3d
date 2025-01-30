#pragma once
#include "board.h"

struct act_t {glm::ivec3 src, dst;};

double static_eval(Board &s);
double minimax(Board &s, int d, double alpha, double beta, int &vis);
act_t best_move(Board &s, int d);
