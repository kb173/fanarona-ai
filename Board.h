#pragma once

#include <array>
#include <string>

const int BOARD_WIDTH = 9;
const int BOARD_HEIGHT = 5;

enum class State : char
{
    EMPTY,
    WHITE,
    BLACK,
};

struct Node
{
    State state = State::EMPTY;
    std::array<Node*, 8> neighbours;
};

class Board
{
  public:
    Board();
    ~Board();

    void parse(std::string boardContent);
    void print();
    std::string getPosition(int); // returns next position, depending on mode: 0 = stone; 1 = free space -> TODO: use
                                  // enums or whatever...; TODO: return type string?

  private:
    Node cells[BOARD_HEIGHT][BOARD_WIDTH] = {};

    inline Node& getCell(int x, int y)
    {
        return cells[y][x];
    }
    bool isPositionInBounds(int x, int y);
};
