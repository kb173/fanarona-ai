#include "AIPlayer.h"
#include "Board.h"

#include <iostream>
#include <limits.h> // for INT_MIN

int AIPlayer::RateBoard(Board& board)
{
  // Get the number of our nodes minus the number of enemy nodes
  int rating = 0;

  // iterate over board
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      std::shared_ptr<Node> node = board.GetCell(x, y);

      if (node->state == EState::WHITE)
      {
        rating++;
      }
      else if (node->state == EState::BLACK)
      {
        rating--;
      }
    }
  }

  return rating;
}

int AIPlayer::Minimax(Board& board,
                      std::shared_ptr<Turn> currentTurn,
                      int depth,
                      int alpha,
                      int beta,
                      EState player)
{
  board.ApplyTurnWithFollowing(currentTurn);

  if (depth == 0)
  {
    int rating = RateBoard(board);
    board.RollbackTurnWithFollowing(currentTurn);
    return rating;
  }

  if (player == EState::WHITE)
  {
    auto allTurns = board.FindTurns(EState::WHITE);
    if (allTurns.size() == 0)
    {
      int rating = RateBoard(board);
      board.RollbackTurnWithFollowing(currentTurn);
      return rating;
    }

    // Optimization: Sort turns in order to check the likely better turns first, making
    // alpha-beta-pruning quit earlier
    SortTurns(allTurns);

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

    board.RollbackTurnWithFollowing(currentTurn);
    return maxScore;
  }
  else
  {
    auto allTurns = board.FindTurns(EState::BLACK);
    if (allTurns.size() == 0)
    {
      int rating = RateBoard(board);
      board.RollbackTurnWithFollowing(currentTurn);
      return rating;
    }

    // Optimization: Sort turns in order to check the likely better turns first, making
    // alpha-beta-pruning quit earlier
    SortTurns(allTurns);

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

    board.RollbackTurnWithFollowing(currentTurn);
    return minScore;
  }
}

void AIPlayer::SortTurns(std::list<std::shared_ptr<Turn>>& turns)
{
  turns.sort(TurnSmallerThan);
}

bool AIPlayer::TurnSmallerThan(std::shared_ptr<Turn> turn1, std::shared_ptr<Turn> turn2)
{
  return RateTurn(turn1) > RateTurn(turn2);
}

int AIPlayer::RateTurn(std::shared_ptr<Turn> turn)
{
  return turn->capture->capturedNodes.size() + turn->GetTurnChainLength();
}

std::string AIPlayer::GetNextMove(Board& board, EMove move)
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
      auto turns = board.FindTurns(EState::WHITE);

      // Get the optimal turn
      int optimal_value = INT_MIN;

      int i = 0;
      for (const auto& turn : turns)
      {
        i++;
        int score = Minimax(board, turn, m_minimax_depth, INT_MIN, INT_MAX, EState::BLACK);
        std::cerr << i << ".Move: "
                  << "Score: " << score << " " << turn->GetTurnChainLength()
                  << "-Lenght-Chain: " << turn->ChainToString() << std::endl;
        if (score > optimal_value)
        {
          m_turn_to_handle = turn;
          optimal_value    = score;
        }
      }

      std::cerr << "Best move: " << m_turn_to_handle->ChainToString() << " with score "
                << optimal_value << std::endl;
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

void AIPlayer::SetDepth(int depth)
{
  m_minimax_depth = depth;
}