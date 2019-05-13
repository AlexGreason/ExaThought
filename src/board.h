//
// Created by exa on 12/26/18.
//



#ifndef EXATHOUGHT_BOARD_H
#define EXATHOUGHT_BOARD_H
#include <string>
#include "bitboards.h"
#include "consts.h"


class move_log {
public:
    char castle_rights;
    char capture_piece;
    int ep_square;
    int fifty_move;
};

class board {
public:
    char squares[NSQUARES];
    U64 pieces[NPIECES];
    U64 colors[NCOLORS];
    int turn;
    char castle_rights;
    int ep_square;
    int fifty_move;
    int num_moves;


    void clear_board();
    void from_fen(const char *fen);
    void set_square(int color, int piece, int square);
    void print_board();
    void init_startpos();
    void gen_legal_moves(move *moves, int *nmoves);
    void gen_pseudolegal_captures(move *moves, int *nmoves);
    void gen_pseudolegal_moves(move *moves, int *nmoves);
    void gen_pseudolegal_noncaptures(move *moves, int *nmoves);
    void gen_castling_moves(move *moves, int *nmoves);
    U64 all_attacks(int turn);
    bool is_in_check(int turn);
    void apply_move(move m, move_log *log);
    void undo_move(move m, move_log *log);
    void apply_promo_move(move m, move_log *pLog);
    void apply_normal_move(move m, move_log *pLog);
    void apply_castle_move(move m, move_log *pLog);
    void apply_enpass_move(move m, move_log *pLog);
    void undo_promo_move(move m, move_log *pLog);
    void undo_normal_move(move m, move_log *pLog);
    void undo_castle_move(move m, move_log *pLog);
    void undo_enpass_move(move m, move_log *pLog);
    bool verify_board();
    void print_move(move m, char* str);
    void print_san(move m, char *str, move *moves, int nmoves);
    move parse_san(std::string san);
};




#endif //EXATHOUGHT_BOARD_H
