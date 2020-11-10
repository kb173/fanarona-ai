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

enum class Mode : char
{
    SELECT_INVALID,
    SELECT_STONE,
    SELECT_MOVEMENT,
    SELECT_CAPTURE,
};

struct Node
{
    State state = State::EMPTY;
    std::array<Node*, 8> neighbours = {};
};

class Move
{
  private:
    Node* origin;

  public:
    Move(Node* origin, int direction) : origin(origin), direction(direction)
    {
    }

    Node* from() const;
    Node* to() const;

    int direction;
};

class Board
{
  public:
    Board();
    ~Board();

    void parse(std::string boardContent);
    void print();

    // returns the next position to send to the server, depending on the mode (picking a stone, specifying movement,
    // etc.)
    // TODO: return type string?
    std::string getPosition(Mode);

  private:
    Node cells[BOARD_HEIGHT][BOARD_WIDTH] = {};

    inline Node* getCell(int x, int y);
    inline bool isPositionInBounds(int x, int y);

    // TODO: set this correctly after capturing, so we know which piece we have to use to continue @Bella
    Node* movingPiece = nullptr;

    // utility string functions, should probably be deleted for final build
    const std::string moveToString(const Move& move);
    const std::string nodeToPositionString(const Node* node);
    const std::string indexToDirectionString(const int& index);

    // find possible moves and returns a vector of it, moves are pairs of nodes pointer and int denoting direction
    // this means the vector can contain the same node multiple times if it can move in different directions
    // always call findMoves, it decides which of the secondary functions to call
    // bool parameter determines for which color we're finding moves
    const std::vector<Move> findMoves(State);

    // secondary find functions because there are two kinds of selection behaviours: normal and continuing moves (after
    // a capture)
    const std::vector<Move> findFirstMoves(State);
    const std::vector<Move> findContinuingMoves();

    // returns all pieces captured by a move, either in the direction of the move or behind the move.
    const std::vector<Node*> getCapturesInDirection(const Move& move, bool reverse_direction);

    // returns all pieces captured by a move in a certain direction.
    // checks forwards and backwards and returns the pieces in the better direction.
    const std::vector<Node*> getBestCaptures(const Move& move);
};
