#include <iostream>
#include "bitboards.h"
#include "board.h"
#include "move.h"
#include "movegen.h"

int main() {
    auto *test = new board();

    //char *fen = const_cast<char *>(startpos);
    //print_bitboard(CASTLING_UNOCCUPIED[BLACK][KINGSIDE]);
    char *fen = const_cast<char *>("r3k3/1P6/8/8/8/8/1p6/R3K3 w Qq - 0 1");
//    move m = make_move(sq_to_index(1, 0), sq_to_index(3, 0));
//    move m2 = make_move(sq_to_index(6, 1), sq_to_index(4, 1));
//    //print_bitboard(PROMO_RANKS);
    move_log m_log{};
    test->from_fen(fen);
    std::cout << test->verify_board() << std::endl;
//    test->apply_move(m, &m_log);
//    test->print_board();
//    test->undo_move(m, &m_log);
//    test->print_board();
    move allmoves[MAXMOVES];
    int nmoves = 0;
    char string[16];
    //test->gen_pseudolegal_moves(allmoves, &nmoves);
    //test->apply_move(4228, &m_log);
    //test->apply_move(2161, &m_log);
    //test->apply_move(1739, &m_log);
    test->print_board();
    divide(test, 3);
//    for(int i = 0; i < nmoves; i++) {
//        test->print_move(allmoves[i], string);
//        std::cout << string << std::endl;
//    }


//    std::cout << test->verify_board() << std::endl;
//    test->apply_move(m, &m_log);
//    test->print_board();
//    std::cout << test->verify_board() << std::endl;
    //print_bitboard(knight_attacks(test->pieces[KNIGHT] & test->colors[WHITE] & BFILE));
    //std::cout << test->is_in_check(WHITE) << " " << test->is_in_check(BLACK) << std::endl;
    return 0;
}