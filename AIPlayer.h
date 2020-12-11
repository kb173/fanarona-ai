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

  int m_nTurnsPlayed                    = 0;
  std::shared_ptr<Turn> m_pTurnToHandle = nullptr;
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

  int RateBoard(Board& board, EState eCurrentPlayer);

  // ////////////////////////////////////////////////////////////////////////
  // Minimax
  //
  // Minimax algorithm that returns a score for a turn

  int Minimax(Board& board,
              std::shared_ptr<Turn> pCurrentTurn,
              int nDepth,
              int nAlpha,
              int nBeta,
              EState eMaximizingPlayer);

  // ////////////////////////////////////////////////////////////////////////
  // SortTurns
  //
  // Sorts a list of turn according to a very simple heuristik, used to speed
  // up minimax

  void SortTurns(std::list<std::shared_ptr<Turn>>& listTurns);

  // ////////////////////////////////////////////////////////////////////////
  // TurnSmallerThan
  //
  // Comparision method used by SortTurns

  static bool TurnSmallerThan(std::shared_ptr<Turn> pTurn1, std::shared_ptr<Turn> pTurn2);

  // ////////////////////////////////////////////////////////////////////////
  // RateTurn
  //
  // Rates a turn based on the captured nodes and follow up turns, used by
  // TurnSmallerThan

  static int RateTurn(std::shared_ptr<Turn> pTurn);

public:
  // ////////////////////////////////////////////////////////////////////////
  // GetNextMove
  //
  // return:  the best available move according to the ai in form of the string that has to be sent
  // to the server

  std::string GetNextMove(Board& board, EMove eMove);

  // ########################################################################
  // Getter & Setter
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // GetTurnsPlayed (Getter for m_turnsPlayed)

  int GetTurnsPlayed();

  // ////////////////////////////////////////////////////////////////////////
  // SetDepth (Setter for nDepth)

  void SetDepth(int nDepth);
};
