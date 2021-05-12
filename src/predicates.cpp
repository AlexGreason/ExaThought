//
// Created by exa on 5/12/19.
//

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include "predicates.h"
#include "eval.h"
#include "pgn.h"
#include <random>

std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)



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
    move moves[MAXMOVES];
    int nmoves = 0;
    b->gen_legal_moves(moves, &nmoves);
    preds[i++] = b->turn == WHITE; // side
    for(int type = PAWN; type <= QUEEN; type++){
        int uscount = b->count_pieces(type, us);
        int oppcount = b->count_pieces(type, them);
        preds[i++] = uscount;
        preds[i++] = oppcount;
        preds[i++] = uscount - oppcount;
    }
    preds[i++] = b->fifty_move;
    preds[i++] = b->fifty_move % 2;
    preds[i++] = nmoves;
    preds[i++] = b->is_in_check(us);
    int ourmat = mg_val(b->eval_material(us));
    int theirmat = mg_val(b->eval_material(them));
    preds[i++] = ourmat;
    preds[i++] = theirmat;
    preds[i++] = ourmat - theirmat;
    int ouradv = b->pawn_advance(us);
    int theiradv = b->pawn_advance(them);
    preds[i++] = ouradv;
    preds[i++] = theiradv;
    preds[i++] = ouradv - theiradv;
    int ourmaxpawn = b->max_rank(PAWN, us);
    int theirmaxpawn = b->max_rank(PAWN, them);
    preds[i++] = ourmaxpawn;
    preds[i++] = theirmaxpawn;
    preds[i++] = ourmaxpawn - theirmaxpawn;
    int ourrooksopen = b->rook_on_open_file(us);
    int theirrooksopen = b->rook_on_open_file(them);
    preds[i++] = ourrooksopen;
    preds[i++] = theirrooksopen;
    preds[i++] = ourrooksopen - theirrooksopen;
    int ourpassed = b->passed_pawns(us);
    int theirpassed = b->passed_pawns(them);
    preds[i++] = ourpassed;
    preds[i++] = theirpassed;
    preds[i++] = ourpassed - theirpassed;
    //for (int j = 0; j < NPREDS; j++){
    //    std::cout << preds[j] << std::endl;
    //}
    //b->print_board();
    assert(i == NPREDS);
}

void process_game(game* g, int* preds, int nply){
    for (int i = 0; i < nply; i++){
        board b = g->boards[i];
        int tmppreds[NPREDS];
        calc_preds(&b, tmppreds);
        memcpy(preds + (NPREDS * i), tmppreds, sizeof(int) * NPREDS);
    }
    //print_array(preds, nply, NPREDS);
}

void process_game_states(game* g, int* preds, int nply){
    // 64 squares, 1 move counter, 1 halfmove counter, 2 elos, 1 side-to-move, 1 result, 1 fromsquare, 1 tosquare
    for (int i = 0; i < nply; i++){
        board b = g->boards[i];
        int tmppreds[72];
        for(int j = 0; j < NSQUARES; j++) {
            tmppreds[j] = b.squares[j];
        }
        int j = NSQUARES;
        tmppreds[j++] = b.num_moves;
        tmppreds[j++] = b.fifty_move;
        tmppreds[j++] = stoi(g->headers["WhiteElo"]);
        tmppreds[j++] = stoi(g->headers["BlackElo"]);
        tmppreds[j++] = b.turn;
        tmppreds[j++] = parse_result(g->headers["Result"]);
        tmppreds[j++] = from_square(g->moves[i]);
        tmppreds[j] = to_square(g->moves[i]);
        memcpy(preds + (72 * i), tmppreds, sizeof(int) * 72);
    }
    //print_array(preds, nply, NPREDS);
}

void write_game_data(game* g, std::string outfile){
    int nply = g->nply;
    int preds[nply][NPREDS];
    std::ofstream out(outfile, std::fstream::out | std::fstream::app);
    process_game(g, &(preds[0][0]), nply);
    int result = g->result;
    for (int i = 0; i < nply; i++){
        for(int j = 0; j < NPREDS; j++){
            out << preds[i][j];
            if (j != NPREDS - 1){
                out << ",";
            }
        }
        out << ";" << (i % 2 == 0 ? result : -result) << std::endl;
    }
}

void calc_board_data(board* b, std::string outfile, int* preds){
    calc_preds(b, preds);
}

void write_board_states(game* g, std::string outfile){
    // 64 squares, 1 move counter, 1 halfmove counter, 2 elos, 1 side-to-move, 1 result, 1 fromsquare, 1 tosquare
    // 72 total
    int NVALS = 72;
    int nply = g->nply;
    if(nply == 0) {
        return;
    }
    int preds[nply][NVALS];
    std::ofstream out(outfile, std::fstream::out | std::fstream::app);
    process_game_states(g, &(preds[0][0]), nply);
    int result = g->result;
    for (int i = 0; i < nply; i++){
        for(int j = 0; j < NVALS; j++){
            out << preds[i][j];
            if (j != NVALS - 1){
                out << ",";
            }
        }
        out << ";" << (i % 2 == 0 ? result : -result) << std::endl;
    }
}

void write_random_state(game* g, std::string outfile){
    if(g->nply < 20){
        return;
    }
    std::ofstream out(outfile, std::fstream::out | std::fstream::app);
    std::uniform_int_distribution<int> uni(0, g->nply - 1);
    int ply = uni(rng);
    board b = g->boards[ply];
    int result = g->result;
    std::string white = g->headers["WhiteElo"];
    std::string black = g->headers["BlackElo"];
    std::string fen = b.to_fen();
    out << fen << "," << white << "," << black << "," << (ply % 2 == 0 ? result : -result) << std::endl;
}