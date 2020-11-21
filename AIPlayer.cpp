#include "AIPlayer.h"
#include "Board.h"

#include <bits/stdc++.h> // for INT_MIN

int AIPlayer::CalculateTurnScore(Turn* turn)
{
  return turn->capture->capturedNodes.size() * 2 + turn->GetTurnChainLength();
}

int AIPlayer::Minimax(Board* board,
                      Turn* currentTurn,
                      int depth,
                      int alpha,
                      int beta,
                      EState player)
{
  if (depth == 0)
  {
    return CalculateTurnScore(currentTurn);
  }

  board->ApplyTurn(currentTurn);

  if (player == EState::WHITE)
  {
    auto allTurns = board->FindTurns(EState::WHITE);
    if (allTurns.size() == 0)
    {
      board->RollbackTurn(currentTurn);
      return CalculateTurnScore(currentTurn);
    }

    int maxScore = INT_MIN;
    for (auto& childTurn : allTurns)
    {
      auto turnScore = Minimax(board, childTurn, depth - 1, alpha, beta, EState::BLACK);
      if (turnScore > maxScore)
      {
        maxScore = turnScore;
      }

      if (turnScore > alpha)
      {
        alpha = turnScore;
      }
      if (beta <= alpha)
      {
        break;
      }
    }

    board->RollbackTurn(currentTurn);
    return maxScore;
  }
  else
  {
    auto allTurns = board->FindTurns(EState::BLACK);
    if (allTurns.size() == 0)
    {
      board->RollbackTurn(currentTurn);
      return CalculateTurnScore(currentTurn);
    }

    int minScore = INT_MAX;
    for (auto& childTurn : allTurns)
    {
      auto turnScore = Minimax(board, childTurn, depth - 1, alpha, beta, EState::WHITE);
      if (turnScore < minScore)
      {
        minScore = turnScore;
      }

      if (turnScore < beta)
      {
        beta = turnScore;
      }
      if (beta <= alpha)
      {
        break;
      }
    }

    board->RollbackTurn(currentTurn);
    return minScore;
  }
}

std::string AIPlayer::getNextMove(Board* board, EMove move)
{
  std::string input;

  if (m_potentially_done)
  {
    m_potentially_done = false;

    if (move == EMove::DEST_X)
    {
      // We're in a chain of turns, so just use the next one
      m_turn_to_handle = m_turn_to_handle->nextTurn;
    }
    else if (move == EMove::ORIGIN_X)
    {
      auto turns = board->FindTurns(EState::WHITE);

      // Get the optimal turn
      int optimal_value = INT_MIN;

      for (const auto& turn : turns)
      {
        int score = Minimax(board, turn, 3, INT_MIN, INT_MAX, EState::WHITE);
        if (score > optimal_value)
        {
          m_turn_to_handle = turn;
          optimal_value    = score;
        }
      }
    }
  }

  // Check what the server is asking of us and output an appropriate message
  if (move == EMove::ORIGIN_X)
  {
    input = std::to_string(m_turn_to_handle->move->From()->x);
  }
  else if (move == EMove::ORIGIN_Y)
  {
    input = std::to_string(m_turn_to_handle->move->From()->y);
  }
  else if (move == EMove::DEST_X)
  {
    input = std::to_string(m_turn_to_handle->move->To()->x);
  }
  else if (move == EMove::DEST_Y)
  {
    input              = std::to_string(m_turn_to_handle->move->To()->y);
    m_potentially_done = true;
  }
  else if (move == EMove::W_OR_A)
  {
    input              = m_turn_to_handle->IsWithdraw() ? "W" : "A";
    m_potentially_done = true;
  }

  return input;
}