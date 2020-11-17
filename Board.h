#pragma once

#include <array>
#include <list>
#include <string>
#include <vector>

const int BOARD_WIDTH  = 9;
const int BOARD_HEIGHT = 5;

enum class EState : char
{
  EMPTY,
  WHITE,
  BLACK,
  CURRENT,
};

enum class EMove : char
{
  ORIGIN_X,
  ORIGIN_Y,
  DEST_X,
  DEST_Y,
  W_OR_A
};

enum class EMode : char
{
  HUMAN,
  AI,
};

struct Node
{
  int x, y;
  EState state                    = EState::EMPTY;
  std::array<Node*, 8> neighbours = {};

  Node() = default;
  Node (int x, int y) : x (x), y (y)
  {
  }

  std::string ToString() const;
};

struct Move
{
private:
  Node* m_origin;

public:
  Move (Node* origin, int direction) : m_origin (origin), direction (direction)
  {
  }

  std::string ToString() const;

  Node* From() const;
  Node* To() const;

  int direction;
};

struct Capture
{
  Capture (std::vector<Node*> capturedNodes) : capturedNodes (capturedNodes)
  {
  }

  std::string ToString() const;

  std::vector<Node*> capturedNodes;
};

struct Turn
{
  Turn (Move* move, Capture* capture) : move (move), capture (capture), nextTurn (nullptr)
  {
  }

  std::string ToString() const;

  uint GetTurnChainLength() const;

  bool IsWithdraw() const;

  Move* move;
  Capture* capture;
  Turn* nextTurn;
};

class Board
{
public:
  Board (EMode);
  ~Board();

  void Parse (std::string boardContent);
  void Print();

  // returns the next position to send to the server,
  // depending on the mode (picking a stone, specifying movement, etc.)
  // TODO: return type string? better use Point?
  std::string GetPosition (EMove);

  const std::list<Turn*> FindTurnsForNode (EState, Node*);

  const std::list<Turn*> FindTurns (EState);

  void ApplyTurn (Turn* turn);

  void RollbackTurn (Turn* turn);

private:
  EMode m_mode;
  Node m_cells[BOARD_HEIGHT][BOARD_WIDTH] = {};

  Turn* turnToHandle;

  bool potentially_done =
    true; // True if the previous command indicates that we may need a new Turn next time

  inline Node* GetCell (int x, int y);
  inline bool IsPositionInBounds (int x, int y);

  // returns all pieces captured by a move, either in the direction of the move or behind the move.
  const std::vector<Node*> GetCapturesInDirection (const Move& move, bool reverse_direction);

  // returns all pieces captured by a move in a certain direction.
  // checks forwards and backwards and returns the pieces in the better direction.
  const std::vector<Node*> GetBestCaptures (const Move& move);
};
