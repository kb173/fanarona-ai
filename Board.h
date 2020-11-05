#pragma once

#include <string>
#include <vector>

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
    std::vector<Node*> neighbours;
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
    std::vector<Node*> getNeighbours(int x, int y);
    bool isPositionInBounds(int x, int y);
};
