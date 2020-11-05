#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Board.h"

Board::Board()
{
}

Board::~Board()
{
}

void Board::parse(std::string boardContent)
{
    std::istringstream stream(boardContent);
    std::string str;

    std::vector<std::string> lines;
    while (getline(stream, str))
    {
        lines.push_back(str);
    }

    for (size_t i = 0; i < lines.size(); i++)
    {
        std::cout << lines[i] << std::endl;
    }
}

void Board::print()
{
}

// temp implementation for stdin input by user: TODO add logic implementation here
std::string Board::getPosition (int mode)
{
    if (mode == 0)
        std::cout << "select stone: ";
    else if (mode == 1)
        std::cout << "select location: ";

    std::string input;
    //std::cin >> input; // does not parse whitespaces!
    std::getline(std::cin, input);

    // TODO: validate input (for user)
    return input;
}