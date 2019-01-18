//
// Created by exa on 12/26/18.
//

#ifndef EXATHOUGHT_MOVE_H
#define EXATHOUGHT_MOVE_H


#include "consts.h"

static const uint16_t frommask  = 0b0000000000111111;
static const uint16_t tomask    = 0b0000111111000000;
static const uint16_t typemask  = 0b0011000000000000;
static const uint16_t promomask = 0b1100000000000000;
static const int toshift = 6;
static const int typeshift = 12;
static const int promoshift = 14;

int from_square (move m);

int to_square (move m);

int move_type(move m);

int promo_type (move m);

move make_move(int from, int to);

move make_move(int from, int to, int type);

move make_move(int from, int to, int type, int promopiece);

#endif //EXATHOUGHT_MOVE_H
