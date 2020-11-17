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

  // Return the String which the Client should send to the Server next.
  std::string GetPosition (EMove);

private:
  EMode m_mode;
  Node m_cells[BOARD_HEIGHT][BOARD_WIDTH] = {};

  Turn* m_turn_to_handle;

  bool m_potentially_done =
    true; // True if the previous command indicates that we may need a new Turn next time

  // Return a list of all possible turns which the given Node could take.
  const std::list<Turn*> FindTurnsForNode (EState, Node*);

  // Return a list of all possible turns which any Node on the field with the given state could
  // take.
  const std::list<Turn*> FindTurns (EState);

  // Apply a turn to the Board by moving the corresponding Node and removing Captures.
  void ApplyTurn (Turn* turn);

  // Rollback a turn which was previously applied with ApplyTurn.
  void RollbackTurn (Turn* turn);

  inline Node* GetCell (int x, int y);
  inline bool IsPositionInBounds (int x, int y);

  // returns all pieces captured by a move, either in the direction of the move or behind the move.
  const std::vector<Node*> GetCapturesInDirection (const Move& move, bool reverse_direction);
};
