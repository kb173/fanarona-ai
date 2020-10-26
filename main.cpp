#include <iostream>
#include "Client.h"
#include "Board.h"

int main() {
    Board board;
    std::string s = "  0 1 2 3 4 5 6 7 8\n0 .-.-.-.-.-.-.-.-.\n  |\\|/|\\|/|\\|/|\\|/|\n1 .-.-.-.-.-.-.-.-.\n  |/|\\|/|\\|/|\\|/|\\|\n2 .-.-O-.-.-.-.-.-#\n  |\\|/|\\|/|\\|/|\\|/|\n3 .-.-.-.-.-#-.-.-#\n  |/|\\|/|\\|/|\\|/|\\|\n4 .-.-.-.-.-.-.-.-.";
    board.parse(s);
    
    return 0;
}
