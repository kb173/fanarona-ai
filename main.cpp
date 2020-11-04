#include <iostream>
#include <string>

#include "Board.h"
#include "Client.h"

int main() {

    Client client("10.64.99.107", 4455); // Lars' Fanorona Server
    //Client client("127.0.0.1", 8888); // Test server

    while (true)
    {
        std::string recv = client.readString();
        std::cout << recv;

        // TODO: define keywords
        if (recv.find("select stone: ") != std::string::npos ||
            recv.find("select location to move: ") != std::string::npos ||
            recv.find("select stone to take:") != std::string::npos) // TODO: fix missing whitespace on server
        {
            std::string input;
            //std::cin >> input; // does not parse whitespaces!
            std::getline(std::cin, input);

            // TODO: validate input (for user)

            client.writeString(input + "\n");
        }
    }

    Board board;
    std::string s = "  0 1 2 3 4 5 6 7 8\n0 .-.-.-.-.-.-.-.-.\n  |\\|/|\\|/|\\|/|\\|/|\n1 .-.-.-.-.-.-.-.-.\n  "
                    "|/|\\|/|\\|/|\\|/|\\|\n2 .-.-O-.-.-.-.-.-#\n  |\\|/|\\|/|\\|/|\\|/|\n3 .-.-.-.-.-#-.-.-#\n  "
                    "|/|\\|/|\\|/|\\|/|\\|\n4 .-.-.-.-.-.-.-.-.";
    board.parse(s);
    return 0;
}
