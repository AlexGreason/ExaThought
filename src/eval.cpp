//
// Created by exa on 5/12/19.
//

#include <iostream>
#include "eval.h"

const int piecevals[NPIECES] = {S(0U, 0U), S(100U, 100U), S(300U, 300U), S(300U, 300U), S(500U, 500U), S(900U, 900U)};

int board::eval_pos(){
    int res = 0;

    return res;
}

int board::count_pieces(int type, bool color){
    U64 bb = this->pieces[type] & this->colors[color];
    return popcount(bb);
}