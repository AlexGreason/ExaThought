#include <iostream>
#include "bitboards.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "pgn.h"

int main() {
    //std::string str = get_file_contents("/home/exa/Documents/Chess/cutechess/leelagames/ID27-game7.pgn");
    std::string str = get_file_contents("/home/exa/Dropbox/Programming/C Code/CLion/ExaThought/src/enpass.pgn");
    auto* g = parse_pgn(str);
    g->print_game();
    return 0;
}