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
    int line = 0; // used to skip every second line
    while (getline(stream, str))
    {
        if (line++ % 2 == 1) lines.push_back(str);
    }

    for (line = 0; line < lines.size(); line++)
    {
        //int inputRow = y * 2 + 1;
        int inputRow = line;
        int y = line;
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
            // temp fix to ignore over bounds
            bool minRow = inputRow > 0;
            bool minCol = inputColumn > 0;
            bool maxRow = inputRow + 1 < BOARD_HEIGHT;
            bool maxCol = inputColumn + 1 < BOARD_WIDTH;

            if (maxRow && lines[inputRow + 1][inputColumn] == '|')
            {
                neighbours.push_back(&getCell(x, y + 1));
            }
            if (maxCol && lines[inputRow][inputColumn + 1] == '-')
            {
                neighbours.push_back(&getCell(x + 1, y));
            }
            if (maxCol && maxRow && lines[inputRow + 1][inputColumn + 1] == '\\')
            {
                neighbours.push_back(&getCell(x + 1, y + 1));
            }
            if (minCol && minRow && lines[inputRow - 1][inputColumn - 1] == '\\')
            {
                neighbours.push_back(&getCell(x - 1, y - 1));
            }
            if (minCol && lines[inputRow][inputColumn - 1] == '-')
            {
                neighbours.push_back(&getCell(x - 1, y));
            }
            if (maxRow && minCol && lines[inputRow + 1][inputColumn - 1] == '/')
            {
                neighbours.push_back(&getCell(x - 1, y + 1));
            }
            if (minRow && maxCol && lines[inputRow - 1][inputColumn + 1] == '/')
            {
                neighbours.push_back(&getCell(x + 1, y - 1));
            }
            if (minRow && lines[inputRow - 1][inputColumn] == '|')
            {
                neighbours.push_back(&getCell(x, y - 1));
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
