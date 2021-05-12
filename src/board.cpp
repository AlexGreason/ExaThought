//
// Created by exa on 12/26/18.
//

#include <cstring>
#include <iostream>
#include <sstream>
#include "consts.h"
#include "board.h"
#include "move.h"

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

    token = strtok_r(nullptr, " ", &saveptr);
    num_moves = static_cast<int>(strtol(token, nullptr, 10));
}


std::string board::to_fen(){
    std::string res;
    std::stringstream strm(res);
    for (int i = NRANKS - 1; i >= 0; i--) {
        int runlen = 0;
        for (int j = 0; j < NFILES; j++) {
            int square = sq_to_index(i, j);
            U64 piece = this->squares[square];
            if (piece == EMPTY) {
                runlen++;
            } else {
                if (runlen != 0) {
                    strm << runlen;
                    runlen = 0;
                }
                strm << piece_to_char(static_cast<char>(piece));
            }
        }
        if (runlen != 0) {
            strm << runlen;
        }
        if (i != 0) {
            strm << "/";
        }
    }
    strm << " ";
    strm << side_to_char(this->turn);
    strm << " ";
    char chars[2][2] = {{'K', 'Q'}, {'k', 'q'}};
    for(int color = WHITE; color <= BLACK; color++){
        for(int side = KINGSIDE; side <= QUEENSIDE; side++){
            if(this->castle_rights && CASTLEMASKS[color][side]){
                strm << chars[color][side];
            }
        }
    }
    if(!this->castle_rights){
        strm << "-";
    }
    strm << " ";
    if(this->ep_square == -1) {
        strm << "-";
    } else {
        int row = index_to_row(this->ep_square);
        int file = index_to_file(this->ep_square);
        // FIXME: somewhere the row and file semantics are mixed up
        strm << rank_to_char(file);
        strm << file_to_char(row);
    }
    strm << " ";
    strm << this->fifty_move;
    strm << " ";
    strm << this->num_moves;

    return strm.str();
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
    std::cout << "verifying!" << std::endl;
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

move board::parse_san(std::string san){
    int nmoves = 0;
    move moves[MAXMOVES];
    gen_legal_moves(moves, &nmoves);
    char lastchar = san[san.length() - 1];
    while (lastchar == '!' || lastchar == '?' || lastchar == '#' || lastchar == '+'){
        san.pop_back();
        lastchar = san[san.length() - 1];
    }
    for (int i = 0; i < nmoves; i++){
        move tmp = moves[i];
        char string[16];
        print_san(tmp, string, moves, nmoves);
        //std::cout << string << " " << san << std::endl;
        if(strcmp(string, san.c_str()) == 0){
            return tmp;
        }
    }
    std::cout << "parse failed!" << std::endl;
    for (int i = 0; i < nmoves; i++){
        move tmp = moves[i];
        char string[16];
        print_san(tmp, string, moves, nmoves);
        if(isupper(san[0])){
            string[0] = static_cast<char>(toupper(string[0]));
        }
        std::cout << string << " " << san << std::endl;
    }
    return 0;
}
void board::print_san(move m, char* str, move* moves, int nmoves){
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char type = squares[fromsquare];
    bool iscapture = squares[tosquare] != EMPTY || move_type(m) == ENPASS_MOVE;
    int i = 0;
    bool samefile = false;
    bool samerow = false;
    bool specsomething = false;
    for(int j = 0; j < nmoves; j++){
        move tmp = moves[j];
        if(to_square(tmp) == to_square(m)){
            int tmpfrom = from_square(tmp);
            if(fromsquare != tmpfrom){
                char tmptype = squares[tmpfrom];
                if(tmptype == type){
                    int file = index_to_file(fromsquare);
                    int row = index_to_row(fromsquare);
                    int tmpfile = index_to_file(tmpfrom);
                    int tmprow = index_to_row(tmpfrom);
                    if(file == tmpfile){
                        samefile = true;
                    }
                    if (row == tmprow){
                        samerow = true;
                    }
                    specsomething = true;
                }
            }
        }
    }
    bool specrow = samefile;
    bool specfile = samerow || (!specrow && specsomething);
    if(move_type(m) == CASTLE_MOVE){
        int len = 0;
        char *cstr;
        if(tosquare == CASTLESQUARES[piece_color(type)][KINGSIDE]) {
            cstr = const_cast<char*>("O-O");
            len = 3;
        } else {
            cstr = const_cast<char*>("O-O-O");
            len = 5;
        }
        for (int j = 0; j < len; j++) {
            str[j] = cstr[j];
        }
        str[len] = 0;
        return;
    }
    if(piece_type(type) != PAWN) {
        str[i++] = static_cast<char>(toupper(piece_to_char(type)));
        if(specfile){
            str[i++] = static_cast<char>('a' + index_to_file(fromsquare));
        }
        if(specrow){
            str[i++] = static_cast<char>('1' + index_to_row(fromsquare));
        }
        if (iscapture){
            str[i++] = 'x';
        }
    } else if (iscapture){
        str[i++] = static_cast<char>('a' + index_to_file(fromsquare));
        str[i++] = 'x';
    }
    str[i++] = static_cast<char>('a' + index_to_file(tosquare));
    str[i++] = static_cast<char>('1' + index_to_row(tosquare));

    if (move_type(m) == ENPASS_MOVE) {
        //str[i++] = 'e';
    } else if (move_type(m) == PROMO_MOVE) {
        str[i++] = '=';
        str[i++] = piece_to_char(make_piece(promo_type(m), WHITE));
    }

    str[i] = 0;
}