#include <iostream>
#include "bitboards.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "pgn.h"

int main() {
    std::string filename = "/u/exa/Documents/chess/TCEC_Season_14_-_Superfinal.pgn";
    std::string outfile = "/u/exa/CLionProjects/ExaThought/src/outfile.txt";
    //std::string filename = "/media/exa/4TB-Archival2/lichessgames/lichess_db_standard_rated_2019-03.pgn";
    parse_file(filename, 1, outfile);
    //std::cout << sizeof(board) << std::endl;
    return 0;
}