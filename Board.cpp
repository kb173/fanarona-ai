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
        // Remove carriage return if a server with Windows line endings is used
        if (str[0] == '\r' && str.length() > 0)
        {
            str = str.substr(1);
        }
        lines.push_back(str);
    }

    for (int y = 0;; y++)
    {
        int inputRow = y * 2 + 1;
        auto line = lines[inputRow];

        if (inputRow >= lines.size())
        {
            break;
        }

        std::cout << line << std::endl;

        for (size_t x = 0; x < line.size(); x++)
        {
            int inputColumn = x * 2 + 2;
            char character = line[inputColumn];

            if (!isPositionInBounds(x, y))
            {
                break;
            }

            auto cell = getCell(x, y);
            if (character == 'O')
            {
                cell->state = State::WHITE;
            }
            else if (character == '#')
            {
                cell->state = State::BLACK;
            }

            if (isPositionInBounds(x - 1, y - 1) && lines[inputRow - 1][inputColumn - 1] == '\\')
            {
                cell->neighbours[0] = getCell(x - 1, y - 1);
            }
            if (isPositionInBounds(x, y - 1) && lines[inputRow - 1][inputColumn] == '|')
            {
                cell->neighbours[1] = getCell(x, y - 1);
            }
            if (isPositionInBounds(x + 1, y - 1) && lines[inputRow - 1][inputColumn + 1] == '/')
            {
                cell->neighbours[2] = getCell(x + 1, y - 1);
            }
            if (isPositionInBounds(x - 1, y) && lines[inputRow][inputColumn - 1] == '-')
            {
                cell->neighbours[3] = getCell(x - 1, y);
            }
            if (isPositionInBounds(x + 1, y) && lines[inputRow][inputColumn + 1] == '-')
            {
                cell->neighbours[4] = getCell(x + 1, y);
            }
            if (isPositionInBounds(x - 1, y + 1) && lines[inputRow + 1][inputColumn - 1] == '/')
            {
                cell->neighbours[5] = getCell(x - 1, y + 1);
            }
            if (isPositionInBounds(x, y + 1) && lines[inputRow + 1][inputColumn] == '|')
            {
                cell->neighbours[6] = getCell(x, y + 1);
            }
            if (isPositionInBounds(x + 1, y + 1) && lines[inputRow + 1][inputColumn + 1] == '\\')
            {
                cell->neighbours[7] = getCell(x + 1, y + 1);
            }
        }
    }
}

Node* Board::getCell(int x, int y)
{
    if (!isPositionInBounds(x, y))
    {
        return nullptr;
    }
    return &cells[y][x];
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
