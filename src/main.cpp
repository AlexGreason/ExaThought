#include <iostream>
#include "bitboards.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "pgn.h"

int main() {
    //std::string str = get_file_contents("/home/exa/Documents/Chess/cutechess/leelagames/ID27-game7.pgn");
    std::string filename = "/home/exa/Dropbox/Programming/C Code/CLion/ExaThought/src/enpass.pgn";
    parse_file(filename);
    return 0;
}