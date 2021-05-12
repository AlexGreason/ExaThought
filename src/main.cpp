#include <iostream>
#include "bitboards.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "pgn.h"
#include "eval.h"

int main() {
    //std::string filename = "/u/exa/Documents/chess/TCEC_Season_14_-_Superfinal.pgn";
    std::string outfile = "/home/exa/Downloads/randstates.txt";
    std::string filename = "/media/exa/4TB-Archival2/lichessgames/lichess_db_standard_rated_2019-03.pgn";
    //parse_file(filename, 0, outfile, true, 0);
    //"r2qkb1r/4ppp1/n2p1n1p/p2P4/1pP1P3/1P1BQ2P/3B1PP1/RN3RK1 b KQkq - 3 28,1394,1402,-1"
    std::cout << "started" << std::endl;
//    for(int i = 0; i < NFILES; i++){
//        print_bitboard(pawn_span(i));
//    }

    parse_randstates(outfile, 0, "/home/exa/Downloads/randstates_comp_extra.txt", true, 1800);
    //std::cout << sizeof(board) << std::endl;
    //board b{};
    //b.from_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 12 5");
    //b.to_fen();
    return 0;
}