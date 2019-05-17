//
// Created by exa on 5/12/19.
//

#ifndef EXATHOUGHT_EVAL_H
#define EXATHOUGHT_EVAL_H

#include "board.h"


#define mg_mask 0x0000FFFF
#define eg_mask 0xFFFF0000
#define S(mg,eg) ((mg)+((eg)<<16U))
#define mg_val(score) ((score)&mg_mask)
#define eg_val(score) ((score)&eg_mask)


#endif //EXATHOUGHT_EVAL_H
