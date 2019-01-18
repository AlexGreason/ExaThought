//
// Created by exa on 12/26/18.
//

#include <iostream>
#include "movegen.h"
#include "board.h"
#include "move.h"

U64 board::all_attacks(int turn){
    U64 empty = ~(colors[WHITE] | colors[BLACK]);
    U64 res = 0;
    res |= pawn_attacks(colors[turn] & pieces[PAWN], turn);
    res |= king_attacks(colors[turn] & pieces[KING]);
    res |= knight_attacks(colors[turn] & pieces[KNIGHT]);
    res |= rook_attacks(colors[turn] & (pieces[ROOK] | pieces[QUEEN]), empty);
    res |= bishop_attacks(colors[turn] & (pieces[BISHOP] | pieces[QUEEN]), empty);
    return res & ~colors[turn];
}

bool board::is_in_check(int turn){
    U64 attacks = all_attacks(!turn);
    return static_cast<bool>(pieces[KING] & colors[turn] & attacks);
}

void board::gen_castling_moves(move *moves, int *nmoves) {
    U64 attacks = all_attacks(!turn);
    U64 pieces = colors[WHITE] | colors[BLACK];
    for(int side = KINGSIDE; side <= QUEENSIDE; side++) {
        if(check_castle_rights(castle_rights, turn, side)) {
            if (!(attacks & CASTLING_NOCHECK[turn][side])) {
                if(!(pieces & CASTLING_UNOCCUPIED[turn][side])) {
                    moves[(*nmoves)++] = make_move(KINGSQUARE[turn], CASTLESQUARES[turn][side], CASTLE_MOVE);
                }
            }
        }
    }

}

void gen_pawn_captures(U64 pawns, U64 empty, U64 targets, int color, move *moves, int *nmoves) {
    int sq = 0;
    while(pawns) {
        sq = pop_lsb(&pawns);
        U64 attacks = pawn_attacks(U64_1 << sq, color);
        attacks &= targets;
        while(attacks) {
            int tosquare = pop_lsb(&attacks);
            if((U64_1 << tosquare) & PROMO_RANKS){
                for (int promo = QUEEN - KNIGHT; promo >= 0; promo--) {
                    moves[(*nmoves)++] = make_move(sq, tosquare, PROMO_MOVE, promo);
                }
            } else {
                moves[(*nmoves)++] = make_move(sq, tosquare);
            }

        }
    }
}

void gen_pawn_moves(U64 pawns, U64 empty, U64 targets, int color, move *moves, int *nmoves) {
    int sq = 0;
    while(pawns) {
        sq = pop_lsb(&pawns);
        U64 attacks = pawn_moves(U64_1 << sq, color);
        attacks |= pawn_moves(empty & attacks & (color==WHITE ? RANK3 : RANK6), color);
        attacks &= empty;
        while(attacks) {
            int tosquare = pop_lsb(&attacks);
            if((U64_1 << tosquare) & PROMO_RANKS){
                for (int promo = QUEEN - KNIGHT; promo >= 0; promo--) {
                    moves[(*nmoves)++] = make_move(sq, tosquare, PROMO_MOVE, promo);
                }
            } else {
                moves[(*nmoves)++] = make_move(sq, tosquare);
            }
        }
    }
}

void gen_knight_actions(U64 knights, U64 empty, U64 targets, move *moves, int *nmoves, bool captures) {
    int sq = 0;
    while(knights) {
        sq = pop_lsb(&knights);
        U64 attacks = knight_attacks(U64_1 << sq);
        if(captures) {
            attacks &= targets;
        } else {
            attacks &= empty;
        }
        while(attacks) {
            int tosquare = pop_lsb(&attacks);
            moves[(*nmoves)++] = make_move(sq, tosquare);
        }
    }
}

void gen_bishop_actions(U64 bishops, U64 empty, U64 targets, move *moves, int *nmoves, bool captures) {
    int sq = 0;
    while(bishops) {
        sq = pop_lsb(&bishops);
        U64 attacks = bishop_attacks(U64_1 << sq, empty);
        if(captures) {
            attacks &= targets;
        } else {
            attacks &= empty;
        }
        while(attacks) {
            int tosquare = pop_lsb(&attacks);
            moves[(*nmoves)++] = make_move(sq, tosquare);
        }
    }
}

void gen_rook_actions(U64 rooks, U64 empty, U64 targets, move *moves, int *nmoves, bool captures) {
    int sq = 0;
    while(rooks) {
        sq = pop_lsb(&rooks);
        U64 attacks = rook_attacks(U64_1 << sq, empty);
        if(captures) {
            attacks &= targets;
        } else {
            attacks &= empty;
        }
        while(attacks) {
            int tosquare = pop_lsb(&attacks);
            moves[(*nmoves)++] = make_move(sq, tosquare);
        }
    }
}

void gen_king_actions(U64 kings, U64 empty, U64 targets, move *moves, int *nmoves, bool captures) {
    int sq = 0;
    while(kings) {
        sq = pop_lsb(&kings);
        U64 attacks = king_attacks(U64_1 << sq);
        if(captures) {
            attacks &= targets;
        } else {
            attacks &= empty;
        }
        while(attacks) {
            int tosquare = pop_lsb(&attacks);
            moves[(*nmoves)++] = make_move(sq, tosquare);
        }
    }
}

void gen_ep_captures(U64 pawns, int color, move *moves, int *nmoves, int ep_square) {
    if(ep_square == -1) {
        return;
    }
    U64 attacks = pawn_attacks(U64_1 << ep_square, !color);
    attacks &= pawns;
    while(attacks) {
        int tosquare = pop_lsb(&attacks);
        moves[(*nmoves)++] = make_move(tosquare, ep_square, ENPASS_MOVE);
    }
}

void board::gen_pseudolegal_captures(move *moves, int *nmoves) {
    int us = turn;
    int them = !turn;
    U64 empty = ~(colors[WHITE] | colors[BLACK]);
    U64 targets = colors[them];

    gen_ep_captures(pieces[PAWN] & colors[us], us, moves, nmoves, ep_square);
    gen_pawn_captures(pieces[PAWN] & colors[us], empty, targets, us, moves, nmoves);
    gen_knight_actions(pieces[KNIGHT] & colors[us], empty, targets, moves, nmoves, true);
    gen_bishop_actions((pieces[BISHOP] | pieces[QUEEN]) & colors[us], empty, targets, moves, nmoves, true);
    gen_rook_actions((pieces[ROOK] | pieces[QUEEN]) & colors[us], empty, targets, moves, nmoves, true);
    gen_king_actions(pieces[KING] & colors[us], empty, targets, moves, nmoves, true);
}

void board::gen_pseudolegal_noncaptures(move *moves, int *nmoves) {
    int us = turn;
    int them = !turn;
    U64 empty = ~(colors[WHITE] | colors[BLACK]);
    U64 targets = colors[them];

    gen_pawn_moves(pieces[PAWN] & colors[us], empty, targets, us, moves, nmoves);
    gen_knight_actions(pieces[KNIGHT] & colors[us], empty, targets, moves, nmoves, false);
    gen_bishop_actions((pieces[BISHOP] | pieces[QUEEN]) & colors[us], empty, targets, moves, nmoves, false);
    gen_rook_actions((pieces[ROOK] | pieces[QUEEN]) & colors[us], empty, targets, moves, nmoves, false);
    gen_king_actions(pieces[KING] & colors[us], empty, targets, moves, nmoves, false);
    gen_castling_moves(moves, nmoves);
}

void board::gen_pseudolegal_moves(move *moves, int *nmoves) {
    gen_pseudolegal_captures(moves, nmoves);
    gen_pseudolegal_noncaptures(moves, nmoves);
}

void board::gen_legal_moves(move *moves, int *nmoves) {
    int pmovecount = 0;
    move pmoves[MAXMOVES];
    gen_pseudolegal_moves(pmoves, &pmovecount);
    move_log m{};
    for(int i = 0; i < pmovecount; i++) {
        apply_move(pmoves[i], &m);
        if(!verify_board()){
            std::cout << "Incorrect applymove result!" << std::endl;
        }
        if(!is_in_check(!turn)) {
            moves[(*nmoves)++] = pmoves[i];
        }
        undo_move(pmoves[i], &m);
        if(!verify_board()){
            std::cout << "Incorrect undomove result!" << std::endl;
        }
    }
}

long perft(board *b, int depth) {
    long count = 0;
    move moves[MAXMOVES];
    int nmoves = 0;
    if(depth <= 0) {
        return 1;
    }
    move_log m{};
    if(!b->verify_board()){
        std::cout << "Incorrect board in perft!" << std::endl;
    }
    b->gen_legal_moves(moves, &nmoves);
    if(depth == 1) {
        return nmoves;
    }
    for(int i = 0; i < nmoves; i++) {

        b->apply_move(moves[i], &m);

        if(!b->verify_board()){
            std::cout << "Incorrect applymove result!" << std::endl;
        }
        count += perft(b, depth - 1);
        b->undo_move(moves[i], &m);
        if(!b->verify_board()){
            std::cout << "Incorrect undomove result!" << std::endl;
        }
    }
    return count;
}

void divide(board *b, int depth) {
    move moves[MAXMOVES];
    int nmoves = 0;
    long count = 0;
    long temp = 0;
    move_log m{};
    char string[16];
    b->gen_legal_moves(moves, &nmoves);
    for(int i = 0; i < nmoves; i++) {
        b->print_move(moves[i], string);
        b->apply_move(moves[i], &m);
        temp = perft(b, depth - 1);
        count += temp;
        std::cout << string << "(" << moves[i] << ")" << ": " << temp << std::endl;
        b->undo_move(moves[i], &m);
    }
    std::cout << std::endl << "Total: " << count << std::endl;
}