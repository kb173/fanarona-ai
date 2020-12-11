#include "AIPlayer.h"
#include "Board.h"

#include <iostream>
#include <limits.h> // for INT_MIN

// ////////////////////////////////////////////////////////////////////////
// RateBoard
//
// Rates the board using multiple heuristics

int AIPlayer::RateBoard(Board& board, EState currentPlayer)
{
  // Get the number of our nodes minus the number of enemy nodes
  int nPieceAmountRating     = 0;
  int nMiddleFieldRating     = 0;
  int nDiagonalFieldRating   = 0;
  int nDistanceToEnemyRating = 0;

  // int scatteredPiecesRating = 0;

  // this takes way to long so were not going to use it
  // keeping it for documentationssake
  // int moveAmountRating      = 0;
  // moveAmountRating += board.FindTurns(EState::WHITE).size();
  // moveAmountRating += board.FindTurns(EState::BLACK).size();

  // iterate over board
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      std::shared_ptr<Node> pNode = board.GetCell(x, y);

      if (pNode->state == EState::WHITE)
      {
        nPieceAmountRating++;
        if (pNode->isDiagonalField)
        {
          nDiagonalFieldRating++;
        }
        if (pNode->isMiddleField)
        {
          nMiddleFieldRating++;
        }

        if (board.DistanceToNearestEnemy(pNode) % 2 == 0)
        {
          nDistanceToEnemyRating += currentPlayer == EState::WHITE ? 1 : -1;
        }
        else
        {
          nDistanceToEnemyRating += currentPlayer == EState::WHITE ? -1 : 1;
        }

        // this takes way to long so were not going to use it
        // keeping it for documentationssake
        /*int scattered = 1;
        for (auto neighbour : node->neighbours)
        {
          if (neighbour != nullptr)
          {
            if (neighbour->state == EState::WHITE)
            {
              scattered = -1;
            }
          }
        }
        scatteredPiecesRating += scattered;*/
      }
      else if (pNode->state == EState::BLACK)
      {
        nPieceAmountRating--;
        if (pNode->isDiagonalField)
        {
          nDiagonalFieldRating--;
        }
        if (pNode->isMiddleField)
        {
          nMiddleFieldRating--;
        }

        // this takes way to long so were not going to use it
        // keeping it for documentationssake
        /*int scattered = -1;
        for (auto neighbour : node->neighbours)
        {
          if (neighbour != nullptr)
          {
            if (neighbour->state == EState::BLACK)
            {
              scattered = 1;
            }
          }
        }
        scatteredPiecesRating += scattered;*/
      }
    }
  }

  float fRating = 0;

  // tweak multipliers to change performance
  fRating += (1.0f * nPieceAmountRating);
  fRating += (0.1f * nMiddleFieldRating);
  fRating += (0.5f * nDiagonalFieldRating);
  // rating += (0.0f * moveAmountRating);
  fRating += (0.1f * nDistanceToEnemyRating);
  // rating += (0.05f * scatteredPiecesRating);

  return fRating;
}

// ////////////////////////////////////////////////////////////////////////
// Minimax
//
// Minimax algorithm that returns a score for a turn

int AIPlayer::Minimax(Board& board,
                      std::shared_ptr<Turn> pCurrentTurn,
                      int nDepth,
                      int nAlpha,
                      int nBeta,
                      EState ePlayer)
{
  board.ApplyTurnWithFollowing(pCurrentTurn);

  if (nDepth == 0)
  {
    int nRating = RateBoard(board, ePlayer);
    board.RollbackTurnWithFollowing(pCurrentTurn);
    return nRating;
  }

  if (ePlayer == EState::WHITE)
  {
    auto listAllTurns = board.FindTurns(EState::WHITE);
    if (listAllTurns.size() == 0)
    {
      int nRating = RateBoard(board, ePlayer);
      board.RollbackTurnWithFollowing(pCurrentTurn);
      return nRating;
    }

    // Optimization: Sort turns in order to check the likely better turns first, making
    // alpha-beta-pruning quit earlier
    SortTurns(listAllTurns);

    int nMaxScore = INT_MIN;
    for (auto& pChildTurn : listAllTurns)
    {
      int nTurnScore = Minimax(board, pChildTurn, nDepth - 1, nAlpha, nBeta, EState::BLACK);
      if (nTurnScore > nMaxScore)
      {
        nMaxScore = nTurnScore;
      }

      if (nTurnScore > nAlpha)
      {
        nAlpha = nTurnScore;
      }
      if (nBeta <= nAlpha)
      {
        break;
      }
    }

    board.RollbackTurnWithFollowing(pCurrentTurn);
    return nMaxScore;
  }
  else
  {
    auto listAllTurns = board.FindTurns(EState::BLACK);
    if (listAllTurns.size() == 0)
    {
      int nRating = RateBoard(board, ePlayer);
      board.RollbackTurnWithFollowing(pCurrentTurn);
      return nRating;
    }

    // Optimization: Sort turns in order to check the likely better turns first, making
    // alpha-beta-pruning quit earlier
    SortTurns(listAllTurns);

    int nMinScore = INT_MAX;
    for (auto& pChildTurn : listAllTurns)
    {
      int nTurnScore = Minimax(board, pChildTurn, nDepth - 1, nAlpha, nBeta, EState::WHITE);
      if (nTurnScore < nMinScore)
      {
        nMinScore = nTurnScore;
      }

      if (nTurnScore < nBeta)
      {
        nBeta = nTurnScore;
      }
      if (nBeta <= nAlpha)
      {
        break;
      }
    }

    board.RollbackTurnWithFollowing(pCurrentTurn);
    return nMinScore;
  }
}

// ////////////////////////////////////////////////////////////////////////
// SortTurns
//
// Sorts a list of turn according to a very simple heuristik, used to speed
// up minimax

void AIPlayer::SortTurns(std::list<std::shared_ptr<Turn>>& listTurns)
{
  listTurns.sort(TurnSmallerThan);
}

// ////////////////////////////////////////////////////////////////////////
// TurnSmallerThan
//
// Comparision method used by SortTurns

bool AIPlayer::TurnSmallerThan(std::shared_ptr<Turn> pTurn1, std::shared_ptr<Turn> pTurn2)
{
  return RateTurn(pTurn1) > RateTurn(pTurn2);
}

// ////////////////////////////////////////////////////////////////////////
// RateTurn
//
// Rates a turn based on the captured nodes and follow up turns, used by
// TurnSmallerThan

int AIPlayer::RateTurn(std::shared_ptr<Turn> pTurn)
{
  return pTurn->capture->capturedNodes.size() + pTurn->GetTurnChainLength();
}

// ////////////////////////////////////////////////////////////////////////
// GetNextMove
//
// return:  the best available move according to the ai in form of the string that has to be sent
// to the server

std::string AIPlayer::GetNextMove(Board& board, EMove eMove)
{
  std::string strInput;

  if (m_bPotentiallyDone)
  {
    m_bPotentiallyDone = false;

    if (eMove == EMove::DEST_X)
    {
      // We're in a chain of turns, so just use the next one
      m_pTurnToHandle = m_pTurnToHandle->nextTurn;
    }
    else if (eMove == EMove::ORIGIN_X)
    {
      m_nTurnsPlayed++;
      auto listTurns = board.FindTurns(EState::WHITE);

      // Get the optimal turn
      int nOptimalvalue = INT_MIN;

      int i = 0;
      for (const auto& pTurn : listTurns)
      {
        i++;
        int nScore = Minimax(board, pTurn, m_nMinimaxDepth, INT_MIN, INT_MAX, EState::BLACK);
#ifndef OMIT_OUTPUT
        std::cerr << i << ".Move: "
                  << "Score: " << nScore << " " << pTurn->GetTurnChainLength()
                  << "-Lenght-Chain: " << pTurn->ChainToString() << std::endl;
#endif
        if (nScore > nOptimalvalue)
        {
          m_pTurnToHandle = pTurn;
          nOptimalvalue   = nScore;
        }
      }

#ifndef OMIT_OUTPUT
      std::cerr << "Best move: " << m_pTurnToHandle->ChainToString() << " with score "
                << nOptimalvalue << std::endl;
#endif
    }
  }

  // Check what the server is asking of us and output an appropriate message
  if (eMove == EMove::ORIGIN_X)
  {
    strInput = std::to_string(m_pTurnToHandle->move->From()->x);
  }
  else if (eMove == EMove::ORIGIN_Y)
  {
    strInput = std::to_string(m_pTurnToHandle->move->From()->y);
  }
  else if (eMove == EMove::DEST_X)
  {
    strInput = std::to_string(m_pTurnToHandle->move->To()->x);
  }
  else if (eMove == EMove::DEST_Y)
  {
    strInput           = std::to_string(m_pTurnToHandle->move->To()->y);
    m_bPotentiallyDone = true;
  }
  else if (eMove == EMove::W_OR_A)
  {
    strInput           = m_pTurnToHandle->IsWithdraw() ? "W" : "A";
    m_bPotentiallyDone = true;
  }

  return strInput;
}

// ########################################################################
// Getter & Setter
// ########################################################################

// ////////////////////////////////////////////////////////////////////////
// GetTurnsPlayed (Getter for m_turnsPlayed)

int AIPlayer::GetTurnsPlayed()
{
  return m_nTurnsPlayed;
}
// ////////////////////////////////////////////////////////////////////////
// SetDepth (Setter for m_turnsPlayed)

void AIPlayer::SetDepth(int nDepth)
{
  m_nMinimaxDepth = nDepth;
}