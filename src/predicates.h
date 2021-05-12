//
// Created by exa on 5/12/19.
//

#ifndef EXATHOUGHT_PREDICATES_H
#define EXATHOUGHT_PREDICATES_H

#include "board.h"
#include "pgn.h"



void write_game_data(game* g, std::string outfile);
void write_board_states(game* g, std::string outfile);
void write_random_state(game* g, std::string outfile);
void calc_board_data(board* b, std::string outfile, int* preds);

#endif //EXATHOUGHT_PREDICATES_H
