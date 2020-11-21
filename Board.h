#pragma once

#include <array>
#include <list>
#include <string>
#include <vector>

#include "GameElements.h"

const int BOARD_WIDTH  = 9;
const int BOARD_HEIGHT = 5;

#ifdef _WIN32
typedef unsigned int uint;
#endif

enum class EMode : char
{
  HUMAN,
  AI,
};

class Board
{
public:
  Board(EMode);
  ~Board();

  void Parse(std::string boardContent);
  void Print();

  // Return the String which the Client should send to the Server next.
  std::string GetPosition(EMove);

private:
  EMode m_mode;
  Node m_cells[BOARD_HEIGHT][BOARD_WIDTH] = {};

  Turn* m_turn_to_handle = nullptr;

  bool m_potentially_done =
    true; // True if the previous command indicates that we may need a new Turn next time

  // Return a list of all possible turns which the given Node could take.
  const std::list<Turn*> FindTurnsForNode(EState, Node*, Turn*);

  // Return a list of all possible turns which any Node on the field with the given state could
  // take.
  const std::list<Turn*> FindTurns(EState);

  // Apply a turn to the Board by moving the corresponding Node and removing Captures.
  void ApplyTurn(Turn* turn);

  // Rollback a turn which was previously applied with ApplyTurn.
  void RollbackTurn(Turn* turn);

  int CalculateTurnScore(Turn* turn);

  int Minimax(Turn* currentTurn, int depth, int alpha, int beta, EState maximizingPlayer);

  inline Node* GetCell(int x, int y);
  inline bool IsPositionInBounds(int x, int y);
  bool NodeAlreadyVisited(Turn*, Node*);
  bool NewDirection(Turn*, int);
  // returns all pieces captured by a move, either in the direction of the move or behind the
  // move.
  const std::vector<Node*> GetCapturesInDirection(const Move& move, bool reverse_direction);
};
