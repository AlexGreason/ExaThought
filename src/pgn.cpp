//
// Created by exa on 5/9/19.
//

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <cstring>


#include "pgn.h"

int parse_result(std::string result);

std::string get_file_contents(const char *filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    return nullptr;
}

void print_map(std::unordered_map<std::string, std::string> map){
    for (auto iter = map.begin(); iter != map.end(); iter++){
        std::cout << iter->first << " : " << iter->second << std::endl;
    }
}

game* parse_pgn(std::vector<std::string> *pgn){
    std::string path = "/home/exa/Documents/cutechess/leelagames/ID27-game7.pgn";
    auto* res = static_cast<game*>(std::calloc(1, sizeof(game)));
    res->headers = std::unordered_map<std::string, std::string>();
    std::string subline;
    board b = board();
    b.init_startpos();
    std::vector<move> moves = std::vector<move>();
    std::vector<board> boards = std::vector<board>();
    int nmoves = 0;
    for(auto line : *pgn){
        if (line[0] == '['){
            line.pop_back();
            line.erase(line.begin());
            unsigned long spaceindex = line.find(' ');
            res->headers.insert({line.substr(0, spaceindex), line.substr(spaceindex + 2, line.length() - spaceindex - 3)});
        } else  if (line.length() > 0){
            std::stringstream linestream(line);
            bool incomment = false;
            while(std::getline(linestream, subline, ' ')){
                bool skip = false;
                if (subline[0] == '{'){
                    incomment = true;
                    skip = true;
                }
                if (subline[subline.length() - 1] == '}'){
                    incomment = false;
                    skip = true;
                }
                if (subline[subline.length() - 1] == '.'){
                    skip = true;
                }
                if(subline[0] == '1' or subline[0] == '0'){
                    skip = true;
                }
                if (!skip and !incomment){
                    move tmp = b.parse_san(subline);
                    move_log log;
                    char string[16];
                    if (tmp != 0){
                        b.print_move(tmp, string);
                        //std::cout << " " << string << std::endl;
                    }
                    nmoves++;
                    moves.push_back(tmp);
                    boards.push_back(b);
                    b.apply_move(tmp, &log);
                    //b.print_board();
                }
            }
        }
    }
    move* movearr = static_cast<move*>(calloc(nmoves, sizeof(move)));
    board* boardarr = static_cast<board*>(calloc(nmoves, sizeof(board)));
    for (int i = 0; i < nmoves; i++){
        memcpy(&movearr[i], &moves[i], sizeof(move));
        memcpy(&boardarr[i], &boards[i], sizeof(board));
    }
    std::string resultstr = res->headers.at("Result");
    res->result = parse_result(resultstr);
    res->nply = nmoves;
    res->moves = movearr;
    res->boards = boardarr;
    //print_map(res->headers);
    return res;
}

int parse_result(std::string result) {
    char first = result[0];
    if (first == '0'){
        return -1;
    }
    char second = result[1];
    if (second == '/'){
        return 0;
    }
    return 1;
}

void game::print_game(){
    print_map(this->headers);
    std::cout << std::endl;
    std::cout << "Result " << this->result << std::endl;
    std::cout << "Ply " << this->nply << std::endl;
}

void game::delete_game(){
    delete this->moves;
    delete this->boards;
    delete this;
}

void parse_file(std::string filename){
    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> pgn = std::vector<std::string>();
    while (std::getline(file, line)){
        pgn.push_back(line);
    }
    auto* g = parse_pgn(&pgn);
    g->print_game();
    g->delete_game();
}