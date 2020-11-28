#pragma once

#include "GameElements.h"

#include <list>

// Forward declarations
class Board;

class AIPlayer
{
private:
  std::shared_ptr<Turn> m_turn_to_handle = nullptr;

  bool m_potentially_done =
    true; // True if the previous command indicates that we may need a new Turn next time

  int m_minimax_depth = 5;

  int RateBoard(Board& board, EState currentPlayer);

  int Minimax(Board& board,
              std::shared_ptr<Turn> currentTurn,
              int depth,
              int alpha,
              int beta,
              EState maximizingPlayer);

  void SortTurns(std::list<std::shared_ptr<Turn>>& turns);

  static bool TurnSmallerThan(std::shared_ptr<Turn> turn1, std::shared_ptr<Turn> turn2);

  static int RateTurn(std::shared_ptr<Turn> turn);

public:
  int turnsFound = 0;
  std::string GetNextMove(Board& board, EMove move);
  void SetDepth(int depth);
};
