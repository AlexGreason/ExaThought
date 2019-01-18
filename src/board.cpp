//
// Created by exa on 12/26/18.
//

#include <cstring>
#include <iostream>
#include "consts.h"
#include "board.h"


static int string_to_square(const char *str) {
    return str[0] == '-' ? -1 : sq_to_index((int)(str[1] - '1'), (int)(str[0] - 'a'));
}

void board::from_fen (const char *fen) {
    clear_board();
    char fencopy[256];
    strncpy(fencopy, fen, 256);
    char *saveptr = nullptr;
    char* token = strtok_r(fencopy, " ", &saveptr);
    size_t tokenlen = strnlen(token, 256);
    int square = 56;
    for(int i = 0; i < tokenlen; i++) {
        char c = token[i];
        if(isdigit(c)) {
            square += c - '0';
        } else if(c == '/') {
            square -= 16;
        } else {
            bool color = islower(c);
            int piece = char_to_piece(tolower(c));
            if(piece >= 0) {
                set_square(color, piece, square);
            }
            square++;
        }
    }

    token = strtok_r(nullptr, " ", &saveptr);
    turn = token[0] == 'w' ? WHITE : BLACK;

    token = strtok_r(nullptr, " ", &saveptr);
    tokenlen = strnlen(token, 5);
    for(int i = 0; i < tokenlen; i++) {
        switch (token[i]) {
            case 'K':
                castle_rights |= WHITE_KINGSIDE_CASTLE;
                break;
            case 'Q':
                castle_rights |= WHITE_QUEENSIDE_CASTLE;
                break;
            case 'k':
                castle_rights |= BLACK_KINGSIDE_CASTLE;
                break;
            case 'q':
                castle_rights |= BLACK_QUEENSIDE_CASTLE;
                break;
            default:
                return;
        }
    }

    token = strtok_r(nullptr, " ", &saveptr);
    ep_square = string_to_square(token);

    token = strtok_r(nullptr, " ", &saveptr);
    fifty_move = static_cast<int>(strtol(token, nullptr, 10));
}

void board::set_square(int color, int piece, int square) {
    int oldpiece = piece_type(squares[square]);
    int oldcolor = piece_color(squares[square]);
    clear_bit(&pieces[oldpiece], square);
    clear_bit(&colors[oldcolor], square);
    squares[square] = make_piece(piece, color);
    set_bit(&pieces[piece], square);
    set_bit(&colors[color], square);
}

void board::clear_board() {
    memset(this, 0, sizeof(board));
    memset(&this->squares, EMPTY, NSQUARES);
    ep_square = -1;
}

void board::print_board() {

    for(int rank = NRANKS - 1; rank >= 0; rank--) {
        std::cout << rank + 1;
        for(int file = 0; file < NFILES; file++) {
            std::cout << piece_to_char(squares[sq_to_index(rank, file)]);
        }
        std::cout << std::endl;
    }
    std::cout << " abcdefgh" << std::endl;
}

void board::init_startpos() {
    from_fen(startpos);
}

bool board::verify_board() {
    for(int i = 0; i < NSQUARES; i++) {
        char piece = squares[i];
        for(int j = 0; j < NPIECES; j++) {
            if (test_bit(pieces[j], i) != (j == piece_type(piece))) {
                return false;
            }
        }
        for(int j = 0; j < NCOLORS; j++) {
            if ((test_bit(colors[j], i) != (j == piece_color(piece)) & !(piece_type(piece) == EMPTY_TYPE))) {
                return false;
            }
            if ((piece_type(piece) == EMPTY_TYPE) & test_bit(colors[j], i)) {
                return false;
            }
        }
    }
    return true;
}