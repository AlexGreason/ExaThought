//
// Created by exa on 5/12/19.
//

#include <cassert>
#include "predicates.h"
#include "board.h"
#include "pgn.h"

void calc_preds(board* b, int preds[NPREDS]){
    int i = 0;
    bool us = static_cast<bool>(b->turn);
    bool them = !us;
    preds[i++] = b->turn; // side
    preds[i++] = b->count_pieces(PAWN, us);
    preds[i++] = b->count_pieces(PAWN, them);
    assert(i == NPREDS);
}

void process_game(game* g, int* preds, int nply){
    
}

