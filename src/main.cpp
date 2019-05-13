#include <iostream>
#include "bitboards.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "pgn.h"

int main() {
    //std::string filename = "/home/exa/Dropbox/Programming/C Code/CLion/ExaThought/src/enpass.pgn";
    std::string filename = "/media/exa/4TB-Archival2/lichessgames/lichess_db_standard_rated_2013-01.pgn";
    parse_file(filename, -1);
    return 0;
}