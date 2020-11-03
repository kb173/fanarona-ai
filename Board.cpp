#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include "Board.h"

Board::Board() {

}

Board::~Board() {

}

void Board::parse(std::string boardContent) {
    std::istringstream stream(boardContent);
    std::string str;

    std::vector<std::string> lines;
    while (getline(stream, str)) {
        lines.push_back(str);
    }

    for (size_t i = 0; i < lines.size(); i++) {
        std::cout << lines[i] << std::endl;
    }
}
