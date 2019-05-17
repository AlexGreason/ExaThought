//
// Created by exa on 12/26/18.
//

#ifndef EXATHOUGHT_BITBOARDS_H
#define EXATHOUGHT_BITBOARDS_H

#include "consts.h"

bool test_bit (U64 bb, int index);
void print_bitboard (U64 bb);
void set_bit (U64 *bb, int index);
void clear_bit (U64 *bb, int index);
void assign_bit (U64 *bb, int index, bool val);
int popcount (U64 bb);
int get_lsb(U64 bb);
int get_msb(U64 bb);
int pop_lsb(U64 *bb);
int sq_to_index (int row, int file);
int index_to_row (int i);
int index_to_file (int i);
int relative_rank (int i, bool color);
int relative_index (int i, bool color);
U64 north_attacks (U64 pieces, U64 empty);
U64 south_attacks (U64 pieces, U64 empty);
U64 vert_attacks(U64 pieces, U64 empty);
U64 east_attacks (U64 pieces, U64 empty);
U64 west_attacks (U64 pieces, U64 empty);
U64 horiz_attacks (U64 pieces, U64 empty);
U64 rook_attacks(U64 rooks, U64 empty);
U64 bishop_attacks(U64 bishops, U64 empty);
U64 pawn_attacks(U64 pawns, int color);
U64 pawn_moves(U64 pawns, int color);
U64 king_attacks(U64 kings);
U64 knight_attacks(U64 knights);

#endif //EXATHOUGHT_BITBOARDS_H
