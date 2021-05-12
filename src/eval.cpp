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

int board::max_rank(int type, bool color){
    U64 pieces = this->pieces[type] & this->colors[color];
    int max = 0;
    while (pieces){
        int i = pop_lsb(&pieces);
        int rank = relative_rank(i, color);
        if (rank > max){
            max = rank;
        }
    }
    return max;
}

U64 pawn_span(int file){
    U64 res = 0;
    res |= FILES[file];
    if(file > 0){
        res |= FILES[file - 1];
    }
    if(file < NFILES - 1){
        res |= FILES[file + 1];
    }
    return res;
}

int board::passed_pawns(bool color){
    U64 pawns = this->pieces[PAWN] & this->colors[color];
    U64 opp_pawns = this->pieces[PAWN] & this->colors[!color];
    int res = 0;
    while(pawns){
        int i = pop_lsb(&pawns);
        int f = index_to_file(i);
        U64 span = pawn_span(f);
        if(!(span & opp_pawns)){
            res++;
        }
    }
    return res;
}

U64 board::open_files(){
    U64 res = 0;
    for(U64 file : FILES) {
        if(!(file & this->pieces[PAWN])){
            res |= file;
        }
    }
    return res;
}

int board::rook_on_open_file(bool color){
    U64 open = this->open_files();
    U64 open_rooks = open & this->pieces[ROOK] & this->colors[color];
    return popcount(open_rooks);
}