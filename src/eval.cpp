//
// Created by exa on 5/12/19.
//

#include <iostream>
#include "eval.h"

const int piecevals[NPIECES] = {S(100U, 100U), S(300U, 300U), S(300U, 300U), S(500U, 500U), S(900U, 900U)};

int board::eval_pos(){
    int res = 0;

    return res;
}

int board::eval_material(bool color){
    int sum = 0;
    for(int type = PAWN; type <= QUEEN; type++){
        sum += piecevals[type] * count_pieces(type, color);
    }
    return sum;
}

int board::count_pieces(int type, bool color){
    U64 bb = this->pieces[type] & this->colors[color];
    return popcount(bb);
}

int board::pawn_advance(bool color){
    U64 pawns = this->pieces[PAWN] & this->colors[color];
    int sum = 0;
    int npawns = count_pieces(PAWN, color);
    while (pawns){
        int i = pop_lsb(&pawns);
        sum += relative_rank(i, color);
    }
    if (npawns > 0){
        return (sum * 10)/npawns;
    }
    return 0;
}