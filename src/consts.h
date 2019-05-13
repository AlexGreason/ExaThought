//
// Created by exa on 12/26/18.
//


#ifndef EXATHOUGHT_CONSTS_H
#define EXATHOUGHT_CONSTS_H
#include <cctype>
#include <cstdint>

#define NSQUARES 64
#define NPIECES 6
#define NCOLORS 2
#define NRANKS 8
#define NFILES 8
#define U64_1 1ULL
#define MAXMOVES 512
typedef unsigned long U64;
typedef uint16_t move;

const U64 AFILE = 0x0101010101010101;
const U64 BFILE = 0x0202020202020202;
const U64 CFILE = 0x0404040404040404;
const U64 DFILE = 0x0808080808080808;
const U64 EFILE = 0x1010101010101010;
const U64 FFILE = 0x2020202020202020;
const U64 GFILE = 0x4040404040404040;
const U64 HFILE = 0x8080808080808080;
const U64 FILES[] = {AFILE, BFILE, CFILE, DFILE, EFILE, FFILE, GFILE, HFILE};
const U64 RANK1 = 0x00000000000000FF;
const U64 RANK2 = 0x000000000000FF00;
const U64 RANK3 = 0x0000000000FF0000;
const U64 RANK4 = 0x00000000FF000000;
const U64 RANK5 = 0x000000FF00000000;
const U64 RANK6 = 0x0000FF0000000000;
const U64 RANK7 = 0x00FF000000000000;
const U64 RANK8 = 0xFF00000000000000;
const U64 RANKS[] = {RANK1, RANK2, RANK3, RANK4, RANK5, RANK6, RANK7, RANK8};
const U64 PROMO_RANKS = RANK1 | RANK8;
const U64 INIT_PAWN_RANK[] = {RANK2, RANK7};
const U64 CASTLING_NOCHECK[2][2] =    {{0x0000000000000070, 0x000000000000001C}, {0x7000000000000000, 0x1C00000000000000}};
const U64 CASTLING_UNOCCUPIED[2][2] = {{0x0000000000000060, 0x000000000000000E}, {0x6000000000000000, 0x0E00000000000000}};
const int CASTLESQUARES[2][2] = {{6, 2}, {62, 58}};
const int KINGSQUARE[2] = {4, 60};

static const char *startpos = const_cast<char*>("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

enum {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6,
    H = 7
};

enum {
    NORMAL_MOVE = 0,
    CASTLE_MOVE = 1,
    ENPASS_MOVE = 2,
    PROMO_MOVE = 3
};

enum {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    EMPTY_TYPE = 6
};

enum {
    WHITE_PAWN   =  0,
    BLACK_PAWN   =  1,
    WHITE_KNIGHT =  4,
    BLACK_KNIGHT =  5,
    WHITE_BISHOP =  8,
    BLACK_BISHOP =  9,
    WHITE_ROOK   = 12,
    BLACK_ROOK   = 13,
    WHITE_QUEEN  = 16,
    BLACK_QUEEN  = 17,
    WHITE_KING   = 20,
    BLACK_KING   = 21,
    EMPTY        = 24
};

enum {
    WHITE = 0,
    BLACK = 1
};

enum {
    WHITE_KINGSIDE_CASTLE = 0b1,
    WHITE_QUEENSIDE_CASTLE = 0b10,
    BLACK_KINGSIDE_CASTLE = 0b100,
    BLACK_QUEENSIDE_CASTLE = 0b1000
};

const char CASTLEMASKS[2][2] = {{WHITE_KINGSIDE_CASTLE, WHITE_QUEENSIDE_CASTLE},
                                {BLACK_KINGSIDE_CASTLE, BLACK_QUEENSIDE_CASTLE}};

enum {
    KINGSIDE = 0,
    QUEENSIDE = 1
};

static __always_inline int piece_type (char piece) {
    return piece / 4;
}

static __always_inline int piece_color (char piece) {
    return piece % 4;
}

static __always_inline char make_piece (int piece, int color) {
    return static_cast<char>(piece * 4 + color);
}

static __always_inline bool check_castle_rights(char rights, int color, int side) {
    return rights & CASTLEMASKS[color][side];
}

static int char_to_piece(int piece) {
    switch (piece) {
        case 'p':
            return PAWN;
        case 'n':
            return KNIGHT;
        case 'b':
            return BISHOP;
        case 'r':
            return ROOK;
        case 'k':
            return KING;
        case 'q':
            return QUEEN;
        default:
            return -1;
    }
}

static char piece_to_char(char piece) {
    int color = piece_color(piece);
    char res;
    switch (piece_type(piece)) {
        case PAWN:
            res = 'p';
            break;
        case KNIGHT:
            res = 'n';
            break;
        case BISHOP:
            res = 'b';
            break;
        case ROOK:
            res = 'r';
            break;
        case QUEEN:
            res = 'q';
            break;
        case KING:
            res = 'k';
            break;
        case EMPTY/4:
            res = '.';
            break;
        default:
            res = '?';
            break;
    }
    if(color == WHITE) {
        res = static_cast<char>(toupper(static_cast<int>(res)));
    }
    return res;
}

#endif //EXATHOUGHT_CONSTS_H
