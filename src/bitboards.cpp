//
// Created by exa on 12/26/18.
//

#include <iostream>
#include <assert.h>
#include "bitboards.h"



void print_bitboard (U64 bb) {
    for(int rank = 0; rank < NRANKS; rank++) {
        std::cout << (7-rank)+1;
        for(int file = 0; file < NFILES; file++) {
            std::cout << (test_bit(bb, file + NFILES * (7-rank)) ? 'X' : '.');
        }
        std::cout << std::endl;
    }
    std::cout << " abcdefgh" << std::endl;
}

bool test_bit (U64 bb, int index) {
    assert(index >= 0 && index < NSQUARES);
    return (bb & (1ULL << index)) != 0;
}

void set_bit(U64 *bb, int index) {
    assert(index >= 0 && index < NSQUARES);
    *bb |= 1ULL << index;
}

void clear_bit (U64 *bb, int index) {
    assert(index >= 0 && index < NSQUARES);
    *bb &= ~(1ULL << index);
}

void assign_bit (U64 *bb, int index, bool val) {
    val ? set_bit(bb, index) : clear_bit(bb, index);
}

int popcount (U64 bb) {
    return __builtin_popcount(bb);
}

int sq_to_index (int row, int file) {
    return file + 8 * row;
}

int index_to_row (int i) {
    return i / 8;
}

int index_to_file (int i) {
    return i % 8;
}

int get_lsb(U64 bb) {
    assert(bb);
    return __builtin_ctzll(bb);
}

int get_msb(U64 bb) {
    assert(bb);
    return __builtin_clzll(bb) ^ 63;
}

int pop_lsb(U64 *bb) {
    int lsb = get_lsb(*bb);
    *bb &= *bb - 1;
    return lsb;
}

U64 north_attacks (U64 pieces, U64 empty) {
    pieces |= empty & (pieces << 8);
    empty &= (empty << 8);
    pieces |= empty & (pieces << 16);
    empty &= (empty << 16);
    pieces |= empty & (pieces << 32);
    pieces |= (pieces << 8);
    return pieces;
}

U64 south_attacks (U64 pieces, U64 empty) {
    pieces |= empty & (pieces >> 8);
    empty &= (empty >> 8);
    pieces |= empty & (pieces >> 16);
    empty &= (empty >> 16);
    pieces |= empty & (pieces >> 32);
    pieces |= (pieces >> 8);
    return pieces;
}

U64 vert_attacks(U64 pieces, U64 empty) {
    return north_attacks(pieces, empty) | south_attacks(pieces, empty);
}

U64 east_attacks (U64 pieces, U64 empty) {
    U64 prop = empty & ~AFILE;
    pieces |= prop & (pieces << 1);
    prop &= prop << 1;
    pieces |= prop & (pieces << 2);
    prop &= prop << 2;
    pieces |= prop & (pieces << 4);
    pieces = ~AFILE & (pieces << 1);
    return pieces;
}

U64 west_attacks (U64 pieces, U64 empty) {
    U64 prop = empty & ~HFILE;
    pieces |= prop & (pieces >> 1);
    prop &= prop >> 1;
    pieces |= prop & (pieces >> 2);
    prop &= prop >> 2;
    pieces |= prop & (pieces >> 4);
    pieces = ~HFILE & (pieces >> 1);
    return pieces;
}

U64 horiz_attacks (U64 pieces, U64 empty) {
    return east_attacks(pieces, empty) | west_attacks(pieces, empty);
}

U64 rook_attacks(U64 rooks, U64 empty) {
    return vert_attacks(rooks, empty) | horiz_attacks(rooks, empty);
}

U64 NE_attacks (U64 pieces, U64 empty) {
    U64 prop = empty & ~AFILE;
    pieces |= prop & (pieces << 9);
    prop &= prop << 9;
    pieces |= prop & (pieces << 18);
    prop &= prop << 18;
    pieces |= prop & (pieces << 36);
    pieces = ~AFILE & (pieces << 9);
    return pieces;
}

U64 NW_attacks (U64 pieces, U64 empty) {
    U64 prop = empty & ~HFILE;
    pieces |= prop & (pieces << 7);
    prop &= prop << 7;
    pieces |= prop & (pieces << 14);
    prop &= prop << 14;
    pieces |= prop & (pieces << 28);
    pieces = ~HFILE & (pieces << 7);
    return pieces;
}

U64 SE_attacks (U64 pieces, U64 empty) {
    U64 prop = empty & ~AFILE;
    pieces |= prop & (pieces >> 7);
    prop &= prop >> 7;
    pieces |= prop & (pieces >> 14);
    prop &= prop >> 14;
    pieces |= prop & (pieces >> 28);
    pieces = ~AFILE & (pieces >> 7);
    return pieces;
}

U64 SW_attacks (U64 pieces, U64 empty) {
    U64 prop = empty & ~HFILE;
    pieces |= prop & (pieces >> 9);
    prop &= prop >> 9;
    pieces |= prop & (pieces >> 18);
    prop &= prop >> 18;
    pieces |= prop & (pieces >> 36);
    pieces = ~HFILE & (pieces >> 9);
    return pieces;
}

U64 bishop_attacks(U64 bishops, U64 empty) {
    return NE_attacks(bishops, empty) |
           NW_attacks(bishops, empty) |
           SE_attacks(bishops, empty) |
           SW_attacks(bishops, empty);
}

U64 white_pawn_attacks(U64 pawns) {
    pawns = pawns << 8;
    U64 west = (pawns >> 1) & ~HFILE;
    U64 east = (pawns << 1) & ~AFILE;
    return west | east;
}

U64 black_pawn_attacks(U64 pawns) {
    pawns = pawns >> 8;
    U64 west = (pawns >> 1) & ~HFILE;
    U64 east = (pawns << 1) & ~AFILE;
    return west | east;
}

U64 pawn_attacks(U64 pawns, int color) {
    return color == WHITE ? white_pawn_attacks(pawns) : black_pawn_attacks(pawns);
}

U64 white_pawn_moves(U64 pawns) {
    return pawns << 8;
}

U64 black_pawn_moves(U64 pawns) {
    return pawns >> 8;
}

U64 pawn_moves(U64 pawns, int color) {
    return color == WHITE ? white_pawn_moves(pawns) : black_pawn_moves(pawns);
}

U64 king_attacks(U64 kings) {
    kings |= kings << 8;
    kings |= (kings << 1) & ~AFILE;
    kings |= kings >> 8;
    kings |= (kings >> 1) & ~HFILE;
    return kings;
}

U64 knight_attacks(U64 knights) {
    U64 northsouth = white_pawn_attacks(knights << 8) | black_pawn_attacks(knights >> 8);
    U64 east = (knights << 2) & ~(AFILE | BFILE);
    U64 west = (knights >> 2) & ~(GFILE | HFILE);
    U64 eastwest = ((east | west) >> 8) | ((east | west) << 8);
    return northsouth | eastwest;
}