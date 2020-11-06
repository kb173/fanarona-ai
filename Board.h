#pragma once

#include <array>
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
    std::array<Node*, 8> neighbours = {};
};

typedef std::pair<Node*,int> move;

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

    inline Node* getCell(int x, int y);
    inline bool isPositionInBounds(int x, int y);

    

    //TODO: set this correctly after capturing, so we know which piece we have to use to continue @Bella
    Node* movingPiece = nullptr;

    //utility string functions, should probably be deleted for final build
    const std::string moveToString(const move & move);
    const std::string nodeToPositionString(const Node* node);
    const std::string indexToDirectionString(const int & index);

    //find possible moves and returns a vector of it, moves are pairs of nodes pointer and int denoting direction
    //this means the vector can contain the same node multiple times if it can move in different directions
    //always call findMoves, it decides which of the secondary functions to call
    //bool parameter determines for which color we're finding moves
    const std::vector<move> findMoves(const bool & white);

    //secondary find functions because there are two kinds of selection behaviours: normal and continuing moves (after a capture)
    const std::vector<move> findFirstMoves(const bool & white);
    const std::vector<move> findContinuingMoves();

    //counts how many piece will be captured by the passed move
    //only checks forward for now
    //TODO: also check backward, decide which one is better @carl
    //think about maybe storing capture direction in move (maybe make it an extra class), because we have to be able to reverse moves so backtracking is possible later
    const int captureCount(const move & move);
};
