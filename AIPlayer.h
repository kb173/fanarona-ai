#pragma once

#include "GameElements.h"

#include <list>

// ########################################################################
// Forward declarations
// ########################################################################
class Board;

class AIPlayer
{
private:
  // ########################################################################
  // Fields
  // ########################################################################
  int m_nTurnsPlayed                     = 0;
  std::shared_ptr<Turn> m_turn_to_handle = nullptr;
  // True if the previous command indicates that we may need a new Turn next time
  bool m_bPotentiallyDone = true;
  int m_nMinimaxDepth     = 5;

  // ########################################################################
  // Methods
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // RateBoard
  //
  // Rates the board using multiple heuristics

  int RateBoard(Board& board, EState currentPlayer);

  // ////////////////////////////////////////////////////////////////////////
  // Minimax
  //
  // Minimax algorithm that returns a score for a turn

  int Minimax(Board& board,
              std::shared_ptr<Turn> currentTurn,
              int depth,
              int alpha,
              int beta,
              EState maximizingPlayer);

  // ////////////////////////////////////////////////////////////////////////
  // SortTurns
  //
  // Sorts a list of turn according to a very simple heuristik, used to speed
  // up minimax

  void SortTurns(std::list<std::shared_ptr<Turn>>& turns);

  // ////////////////////////////////////////////////////////////////////////
  // TurnSmallerThan
  //
  // Comparision method used by SortTurns

  static bool TurnSmallerThan(std::shared_ptr<Turn> turn1, std::shared_ptr<Turn> turn2);

  // ////////////////////////////////////////////////////////////////////////
  // RateTurn
  //
  // Rates a turn based on the captured nodes and follow up turns, used by
  // TurnSmallerThan

  static int RateTurn(std::shared_ptr<Turn> turn);

public:
  // ////////////////////////////////////////////////////////////////////////
  // GetNextMove
  //
  // return:  the best available move according to the ai in form of the string that has to be sent
  // to the server

  std::string GetNextMove(Board& board, EMove move);

  // ########################################################################
  // Getter & Setter
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // GetTurnsPlayed (Getter for m_turnsPlayed)

  int GetTurnsPlayed();

  // ////////////////////////////////////////////////////////////////////////
  // SetDepth (Setter for m_turnsPlayed)

  void SetDepth(int depth);
};
