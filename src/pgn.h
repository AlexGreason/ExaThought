//
// Created by exa on 5/9/19.
//

#ifndef EXATHOUGHT_PGN_H
#define EXATHOUGHT_PGN_H

#include <unordered_map>
#include "move.h"
#include "board.h"
#include <vector>

struct game {
    int nply;
    int result;
    std::unordered_map<std::string, std::string> headers;
    move* moves;
    board* boards;

    void print_game();
    void delete_game();
};

game* parse_pgn(std::vector<std::string> *pgn);
void parse_file(std::string filename, int maxgames, std::string outfile, bool discardunusual);
std::string get_file_contents(const char *filename);


#endif //EXATHOUGHT_PGN_H
