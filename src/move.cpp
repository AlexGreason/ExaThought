//
// Created by exa on 12/26/18.
//

#include "move.h"
#include "board.h"


int __always_inline from_square (move m) {
    return m & frommask;
}

int __always_inline to_square (move m) {
    return (m & tomask) >> toshift;
}

int __always_inline move_type(move m) {
    return (m & typemask) >> typeshift;
}

int __always_inline promo_type (move m) {
    return ((m & promomask) >> promoshift) + KNIGHT;
}

move make_move(int from, int to){
    return static_cast<move>(from | (to << toshift));
}

move make_move(int from, int to, int type) {
    return static_cast<move>(from | (to << toshift) | (type << typeshift));
}

move make_move(int from, int to, int type, int promopiece) {
    return static_cast<move>(from | (to << toshift) | (type << typeshift) | ((promopiece - KNIGHT) << promoshift));
}

void board::print_move(move m, char* str) {
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char type = squares[fromsquare];
    int i = 0;
    if(move_type(m) == CASTLE_MOVE){
        int len = 0;
        char *cstr;
        if(tosquare == CASTLESQUARES[piece_color(type)][KINGSIDE]) {
            cstr = const_cast<char *>("O-O");
            len = 3;
        } else {
            cstr = const_cast<char *>("O-O-O");
            len = 5;
        }
        for (int j = 0; j < len; j++) {
            str[j] = cstr[j];
        }
        str[len] = 0;
        return;
    }
    if(piece_type(type) != PAWN) {
        str[i++] = piece_to_char(type);
    }
    str[i++] = static_cast<char>('a' + index_to_file(tosquare));
    str[i++] = static_cast<char>('1' + index_to_row(tosquare));

    if (move_type(m) == ENPASS_MOVE) {
        str[i++] = 'e';
    } else if (move_type(m) == PROMO_MOVE) {
        str[i++] = piece_to_char(make_piece(promo_type(m), BLACK));
    }

    str[i] = 0;
}

void board::apply_move(move m, move_log *log) {
    log->fifty_move = fifty_move;
    log->ep_square = ep_square;
    log->castle_rights = castle_rights;

    fifty_move++;

    int type = move_type(m);
    switch (type) {
        case NORMAL_MOVE:
            board::apply_normal_move(m, log);
            break;
        case PROMO_MOVE:
            board::apply_promo_move(m, log);
            break;
        case CASTLE_MOVE:
            board::apply_castle_move(m, log);
            break;
        default: // ENPASS_MOVE
            board::apply_enpass_move(m, log);
            break;
    }


    if(log->ep_square == ep_square) {
        ep_square = -1;
    }

    turn = turn == WHITE ? BLACK : WHITE;
}


void board::apply_normal_move(move m, move_log *log) {
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char frompiece = squares[fromsquare];
    char topiece = squares[tosquare];

    int frompiecetype = piece_type(frompiece);
    int frompiececolor = piece_color(frompiece);

    int topiecetype = piece_type(topiece);

    squares[tosquare] = frompiece;
    squares[fromsquare] = EMPTY;

    if(topiecetype != EMPTY_TYPE) {
        colors[!frompiececolor] &= ~(U64_1 << tosquare);
        pieces[topiecetype] &= ~(U64_1 << tosquare);
        fifty_move = 0;
    }

    pieces[frompiecetype] ^= (U64_1 << fromsquare) | (U64_1 << tosquare);
    colors[frompiececolor] ^= (U64_1 << fromsquare) | (U64_1 << tosquare);

    if(frompiecetype == PAWN) {
        int fromrank = index_to_row(fromsquare);
        int torank = index_to_row(tosquare);
        if(torank - fromrank == (2 - 4 * turn)) {
            ep_square = sq_to_index((fromrank + torank)/2, index_to_file(fromsquare));
        }
        fifty_move = 0;
    }

    if(frompiecetype == KING) {
        castle_rights &= ~(CASTLEMASKS[turn][KINGSIDE] | CASTLEMASKS[turn][QUEENSIDE]);
    }

    if(frompiecetype == ROOK) {
        int queenrooksquareours = sq_to_index(turn == WHITE ? 0 : 7, A);
        int kingrooksquareours  = sq_to_index(turn == WHITE ? 0 : 7, H);
        if(fromsquare == queenrooksquareours) {
            castle_rights &= ~CASTLEMASKS[turn][QUEENSIDE];
        }

        if(fromsquare == kingrooksquareours) {
            castle_rights &= ~CASTLEMASKS[turn][KINGSIDE];
        }
    }

    if(topiecetype == ROOK) {
        int queenrooksquaretheirs = sq_to_index(turn == BLACK ? 0 : 7, A);
        int kingrooksquaretheirs  = sq_to_index(turn == BLACK ? 0 : 7, H);
        if(tosquare == queenrooksquaretheirs) {
            castle_rights &= ~CASTLEMASKS[!turn][QUEENSIDE];
        }

        if(tosquare == kingrooksquaretheirs) {
            castle_rights &= ~CASTLEMASKS[!turn][KINGSIDE];
        }
    }

    log->capture_piece = topiece;
}

void board::apply_promo_move(move m, move_log *log) {
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char frompiece = squares[fromsquare];
    char topiece = squares[tosquare];

    int frompiecetype = piece_type(frompiece);
    int frompiececolor = piece_color(frompiece);

    int topiecetype = piece_type(topiece);
    int topiececolor = piece_color(topiece);

    squares[tosquare] = make_piece(promo_type(m), frompiececolor);
    squares[fromsquare] = EMPTY;

    if(topiecetype != EMPTY_TYPE) {
        colors[topiececolor] ^= (U64_1 << tosquare);
        pieces[topiecetype] ^= (U64_1 << tosquare);
    }

    pieces[promo_type(m)] ^= (U64_1 << tosquare);
    pieces[PAWN] ^= (U64_1 << fromsquare);
    colors[frompiececolor] ^= (U64_1 << fromsquare) | (U64_1 << tosquare);


    int queenrooksquaretheirs = sq_to_index(turn == BLACK ? 0 : 7, A);
    int kingrooksquaretheirs  = sq_to_index(turn == BLACK ? 0 : 7, H);
    if(tosquare == queenrooksquaretheirs) {
        castle_rights &= ~CASTLEMASKS[!turn][QUEENSIDE];
    }

    if(tosquare == kingrooksquaretheirs) {
        castle_rights &= ~CASTLEMASKS[!turn][KINGSIDE];
    }

    fifty_move = 0;

    log->capture_piece = topiece;
}

void board::apply_castle_move(move m, move_log *log) {
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char frompiece = squares[fromsquare];
    int turn = piece_color(frompiece);

    int start_rook_square = tosquare + (tosquare > fromsquare ? 1 : -2);
    int end_rook_square = tosquare + (tosquare > fromsquare ? -1 : 1);

    castle_rights &= ~(CASTLEMASKS[turn][KINGSIDE] | CASTLEMASKS[turn][QUEENSIDE]);
    squares[end_rook_square] = squares[start_rook_square];
    log->capture_piece = squares[start_rook_square];
    squares[start_rook_square] = EMPTY;

    squares[tosquare] = squares[fromsquare];
    squares[fromsquare] = EMPTY;

    pieces[ROOK] ^= (U64_1 << start_rook_square) | (U64_1 << end_rook_square);
    pieces[KING] ^= (U64_1 << fromsquare) | (U64_1 << tosquare);
    colors[turn] ^= (U64_1 << start_rook_square) | (U64_1 << end_rook_square) | (U64_1 << fromsquare) | (U64_1 << tosquare);

}

void board::apply_enpass_move(move m, move_log *log) {
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char frompiece = squares[fromsquare];
    int frompiececolor = piece_color(frompiece);

    int capture_square = tosquare + (frompiececolor == WHITE ? -NFILES : NFILES);

    log->capture_piece = squares[capture_square];
    fifty_move = 0;
    squares[capture_square] = EMPTY;
    squares[tosquare] = squares[fromsquare];
    squares[fromsquare] = EMPTY;

    pieces[PAWN] ^= (U64_1 << fromsquare) ^ (U64_1 << tosquare);
    colors[frompiececolor] ^= (U64_1 << fromsquare) ^ (U64_1 << tosquare);

    pieces[PAWN]  ^= (U64_1 << capture_square);
    colors[!frompiececolor] ^= (U64_1 << capture_square);

}

void board::undo_move(move m, move_log *log) {
    fifty_move = log->fifty_move;
    ep_square = log->ep_square;
    castle_rights = log->castle_rights;

    int type = move_type(m);
    switch (type) {
        case NORMAL_MOVE:
            board::undo_normal_move(m, log);
            break;
        case PROMO_MOVE:
            board::undo_promo_move(m, log);
            break;
        case CASTLE_MOVE:
            board::undo_castle_move(m, log);
            break;
        default: // ENPASS_MOVE
            board::undo_enpass_move(m, log);
            break;
    }

    turn = turn == WHITE ? BLACK : WHITE;
}

void board::undo_castle_move(move m, move_log *log) {
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char frompiece = squares[tosquare];
    int fromcolor = piece_color(frompiece);

    int start_rook_square = tosquare + (tosquare > fromsquare ? 1 : -2);
    int end_rook_square = tosquare + (tosquare > fromsquare ? -1 : 1);

    squares[start_rook_square] = log->capture_piece;
    squares[end_rook_square] = EMPTY;

    squares[fromsquare] = squares[tosquare];
    squares[tosquare] = EMPTY;

    pieces[ROOK] ^= (U64_1 << start_rook_square) | (U64_1 << end_rook_square);
    pieces[KING] ^= (U64_1 << fromsquare) | (U64_1 << tosquare);
    colors[fromcolor] ^= (U64_1 << start_rook_square) | (U64_1 << end_rook_square) | (U64_1 << fromsquare) | (U64_1 << tosquare);
}

void board::undo_promo_move(move m, move_log *log) {
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char frompiece = squares[tosquare];
    char topiece = log->capture_piece;

    int frompiecetype = piece_type(frompiece);
    int frompiececolor = piece_color(frompiece);

    int topiecetype = piece_type(topiece);
    int topiececolor = piece_color(topiece);

    squares[tosquare] = log->capture_piece;
    squares[fromsquare] = make_piece(PAWN, frompiececolor);

    if(topiecetype != EMPTY_TYPE) {
        colors[topiececolor] ^= (U64_1 << tosquare);
        pieces[topiecetype] ^= (U64_1 << tosquare);
    }

    if(frompiecetype != EMPTY_TYPE) {
        pieces[promo_type(m)] ^= (U64_1 << tosquare);
        pieces[PAWN] ^= (U64_1 << fromsquare);
        colors[frompiececolor] ^= (U64_1 << fromsquare) | (U64_1 << tosquare);
    }

    int queenrooksquaretheirs = sq_to_index(turn == BLACK ? 0 : 7, A);
    int kingrooksquaretheirs  = sq_to_index(turn == BLACK ? 0 : 7, H);
    if(tosquare == queenrooksquaretheirs) {
        castle_rights &= ~CASTLEMASKS[!turn][QUEENSIDE];
    }

    if(tosquare == kingrooksquaretheirs) {
        castle_rights &= ~CASTLEMASKS[!turn][KINGSIDE];
    }

    fifty_move = 0;

}

void board::undo_normal_move(move m, move_log *log) {
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char frompiece = squares[tosquare];
    char topiece = log->capture_piece;

    int frompiecetype = piece_type(frompiece);
    int frompiececolor = piece_color(frompiece);

    int topiecetype = piece_type(topiece);
    int topiececolor = piece_color(topiece);

    squares[tosquare] = topiece;
    squares[fromsquare] = frompiece;

    if(frompiecetype != EMPTY_TYPE) {
        pieces[frompiecetype] ^= (U64_1 << fromsquare) | (U64_1 << tosquare);
        colors[frompiececolor] ^= (U64_1 << fromsquare) | (U64_1 << tosquare);
    }

    if(topiecetype != EMPTY_TYPE) {
        pieces[topiecetype] |= (U64_1 << tosquare);
        colors[topiececolor] |= (U64_1 << tosquare);
    }

}


void board::undo_enpass_move(move m, move_log *log) {
    int fromsquare = from_square(m);
    int tosquare = to_square(m);
    char frompiece = squares[tosquare];
    int frompiececolor = piece_color(frompiece);

    int capture_square = tosquare + (frompiececolor == WHITE ? -NFILES : NFILES);

    squares[capture_square] = make_piece(PAWN, !frompiececolor);
    squares[fromsquare] = squares[tosquare];
    squares[tosquare] = EMPTY;

    pieces[PAWN] ^= (U64_1 << fromsquare) ^ (U64_1 << tosquare);
    colors[frompiececolor] ^= (U64_1 << fromsquare) ^ (U64_1 << tosquare);

    pieces[PAWN]  ^= (U64_1 << capture_square);
    colors[!frompiececolor] ^= (U64_1 << capture_square);
}

