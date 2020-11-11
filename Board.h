#pragma once

#include <array>
#include <string>
#include <vector>

const int BOARD_WIDTH = 9;
const int BOARD_HEIGHT = 5;

enum class EState : char
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
    int x, y;
    EState state = EState::EMPTY;
    std::array<Node*, 8> neighbours = {};

    Node() = default;
    Node(int x, int y) : x(x), y(y)
    {
    }

    std::string ToString() const;
};

struct Move
{
  private:
    Node* origin;

  public:
    Move(Node* origin, int direction) : origin(origin), direction(direction)
    {
    }

    std::string ToString() const;

    Node* From() const;
    Node* To() const;

    int direction;
};

struct Capture
{
    Capture(std::vector<Node*> capturedNodes) : capturedNodes(capturedNodes)
    {
    }

    std::string ToString() const;

    std::vector<Node*> capturedNodes;
};

struct Turn
{
    Turn(Move* move, Capture* capture) : move(move), capture(capture), nextTurn(nullptr)
    {
    }

    std::string ToString() const;

    Move* move;
    Capture* capture;
    Turn* nextTurn;
};

class Board
{
  public:
    Board();
    ~Board();

    void Parse(std::string boardContent);
    void Print();

    // returns the next position to send to the server, depending on the mode (picking a stone, specifying movement,
    // etc.)
    // TODO: return type string?
    std::string GetPosition(EMode);

    const std::vector<Turn> FindTurns(EState);

  private:
    Node cells[BOARD_HEIGHT][BOARD_WIDTH] = {};

    inline Node* GetCell(int x, int y);
    inline bool IsPositionInBounds(int x, int y);

    // TODO: set this correctly after capturing, so we know which piece we have to use to continue @Bella
    Node* movingPiece = nullptr;

    // find possible moves and returns a vector of it, moves are pairs of nodes pointer and int denoting direction
    // this means the vector can contain the same node multiple times if it can move in different directions
    // always call findMoves, it decides which of the secondary functions to call
    // bool parameter determines for which color we're finding moves
    const std::vector<Move> FindMoves(EState);

    // secondary find functions because there are two kinds of selection behaviours: normal and continuing moves (after
    // a capture)
    const std::vector<Move> FindFirstMoves(EState);
    const std::vector<Move> FindContinuingMoves();

    // returns all pieces captured by a move, either in the direction of the move or behind the move.
    const std::vector<Node*> GetCapturesInDirection(const Move& move, bool reverse_direction);

    // returns all pieces captured by a move in a certain direction.
    // checks forwards and backwards and returns the pieces in the better direction.
    const std::vector<Node*> GetBestCaptures(const Move& move);
};
