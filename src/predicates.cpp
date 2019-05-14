//
// Created by exa on 5/12/19.
//

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include "predicates.h"


void print_array(int* arr, int rows, int cols){
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++){
            std::cout << arr[cols * r + c];
            if (c != cols - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void calc_preds(board* b, int preds[NPREDS]){
    int i = 0;
    bool us = static_cast<bool>(b->turn);
    bool them = !us;
    preds[i++] = b->turn; // side
    preds[i++] = b->count_pieces(PAWN, us);
    preds[i++] = b->count_pieces(PAWN, them);
    assert(i == NPREDS);
}

void process_game(game* g, int* preds, int nply){
    for (int i = 0; i < nply; i++){
        board b = g->boards[i];
        int tmppreds[NPREDS];
        calc_preds(&b, preds);
        memcpy(preds + (NPREDS * i), tmppreds, sizeof(tmppreds));
        print_array(preds, nply, NPREDS);
    }
}

void write_game_data(game* g){
    int nply = g->nply;
    int preds[nply][NPREDS];
    process_game(g, &(preds[0][0]), nply);
    int result = g->result;
    for (int i = 0; i < 2; i++){
        for(int j = 0; j < NPREDS; j++){
            std::cout << preds[i][j];
            if (j != NPREDS - 1){
                std::cout << ",";
            }
        }
        std::cout << ";" << result << std::endl;
    }
}