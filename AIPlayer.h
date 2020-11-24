#pragma once

#include "GameElements.h"

// Forward declarations
class Board;

class AIPlayer
{
private:
  std::shared_ptr<Turn> m_turn_to_handle = nullptr;

  bool m_potentially_done =
    true; // True if the previous command indicates that we may need a new Turn next time

  int m_minimax_depth = 5;

  int RateBoard(std::shared_ptr<Board> board);

  int Minimax(std::shared_ptr<Board> board,
              std::shared_ptr<Turn> currentTurn,
              int depth,
              int alpha,
              int beta,
              EState maximizingPlayer);

public:
  std::string GetNextMove(std::shared_ptr<Board> board, EMove move);
};
