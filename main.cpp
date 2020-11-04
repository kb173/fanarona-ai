#include <iostream>

#include "Board.h"
#include "Client.h"

int main()
{
    Client client("10.64.99.107", 4455);
    std::string recv = client.readString();
    std::cout << "received: " << recv << std::endl;

    Board board;
    std::string s = "  0 1 2 3 4 5 6 7 8\n0 .-.-.-.-.-.-.-.-.\n  |\\|/|\\|/|\\|/|\\|/|\n1 .-.-.-.-.-.-.-.-.\n  "
                    "|/|\\|/|\\|/|\\|/|\\|\n2 .-.-O-.-.-.-.-.-#\n  |\\|/|\\|/|\\|/|\\|/|\n3 .-.-.-.-.-#-.-.-#\n  "
                    "|/|\\|/|\\|/|\\|/|\\|\n4 .-.-.-.-.-.-.-.-.";
    board.parse(s);
    return 0;
}
