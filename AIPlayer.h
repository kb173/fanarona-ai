#pragma once

#include "GameElements.h"

// Forward declarations
class Board;

class AIPlayer
{
private:
  Turn* m_turn_to_handle = nullptr;

  bool m_potentially_done =
    true; // True if the previous command indicates that we may need a new Turn next time

  int CalculateTurnScore(Turn* turn);

  int Minimax(Board* Board,
              Turn* currentTurn,
              int depth,
              int alpha,
              int beta,
              EState maximizingPlayer);

public:
  std::string getNextMove(Board* board, EMove move);
};
