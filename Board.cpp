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

    for (size_t y = 0; y < lines.size(); y++)
    {
        int inputRow = y * 2 + 1;
        auto line = lines[inputRow];
        for (size_t x = 0; x < line.size(); x++)
        {
            int inputColumn = x * 2 + 2;
            char character = line[inputColumn];

            if (!isPositionInBounds(x, y))
            {
                break;
            }
            auto& cell = getCell(x, y);

            if (character == 'O')
            {
                cell.state = State::WHITE;
            }
            else if (character == '#')
            {
                cell.state = State::BLACK;
            }

            std::vector<Node*> neighbours;
            if (lines[inputRow - 1][inputColumn - 1] == '\\')
            {
                neighbours.push_back(&getCell(x - 1, y - 1));
            }
            if (lines[inputRow][inputColumn - 1] == '-')
            {
                neighbours.push_back(&getCell(x - 1, y));
            }
            if (lines[inputRow + 1][inputColumn - 1] == '/')
            {
                neighbours.push_back(&getCell(x - 1, y + 1));
            }
            if (lines[inputRow - 1][inputColumn] == '|')
            {
                neighbours.push_back(&getCell(x, y - 1));
            }
            if (lines[inputRow + 1][inputColumn] == '|')
            {
                neighbours.push_back(&getCell(x, y + 1));
            }
            if (lines[inputRow - 1][inputColumn + 1] == '/')
            {
                neighbours.push_back(&getCell(x + 1, y - 1));
            }
            if (lines[inputRow][inputColumn + 1] == '-')
            {
                neighbours.push_back(&getCell(x + 1, y));
            }
            if (lines[inputRow + 1][inputColumn + 1] == '\\')
            {
                neighbours.push_back(&getCell(x + 1, y + 1));
            }

            cell.neighbours = neighbours;
        }
    }
}

bool Board::isPositionInBounds(int x, int y)
{
    return x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT;
}

// temp implementation for stdin input by user: TODO add logic implementation here
std::string Board::getPosition(int mode)
{
    if (mode == 0)
        std::cout << "select stone: ";
    else if (mode == 1)
        std::cout << "select location: ";

    std::string input;
    // std::cin >> input; // does not parse whitespaces!
    std::getline(std::cin, input);

    // TODO: validate input (for user)
    return input;
}
