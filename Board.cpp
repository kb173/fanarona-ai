#include <iostream>
#include <limits.h>
#include <memory>
#include <sstream>

#include "Board.h"

Board::Board(EMode mode, int depth) : m_mode(mode)
{
  // iterate over board, creating cells
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      m_cells[y][x] = std::make_shared<Node>();
    }
  }

  // setting search depth for ai player
  m_player.SetDepth(depth);
}

void Board::Parse(std::string boardContent)
{
  static bool bFirst_parse = true; // using static variable to determine first parsing phase
  std::istringstream stream(boardContent);
  std::string str;

  std::vector<std::string> lines;
  while (getline(stream, str))
  {
    // Remove carriage return if a server with Windows line endings is used
    if (str[0] == '\r' && str.length() > 0)
    {
      str = str.substr(1);
    }

    // don't add empty lines
    if (str.length() != 0)
    {
      lines.push_back(str);
    }
  }

  for (int y = 0;; y++)
  {
    int inputRow = y * 2 + 1;
    if (inputRow >= (int)lines.size())
    {
      break;
    }

    auto line     = lines[inputRow];
    int iLineSize = (int)line.size();
    for (int x = 0; x < iLineSize; x++)
    {
      if (!IsPositionInBounds(x, y))
      {
        break;
      }

      int inputColumn = x * 2 + (inputRow % 2);
      char character  = line[inputColumn];

      std::shared_ptr<Node> cell = GetCell(x, y);
      cell->x                    = x;
      cell->y                    = y;

      // TODO: define "colors" as constants, add property playerColor
      if (character == '1' || character == '#')
      {
        cell->state = EState::BLACK;
      }
      else if (character == '2' || character == 'O')
      {
        cell->state = EState::WHITE;
      }
      else
      {
        cell->state = EState::EMPTY;
      }

      if (bFirst_parse)
      {
        if (IsPositionInBounds(x - 1, y - 1) &&
            lines[inputRow - 1][inputColumn - 1 - (inputRow % 2)] == '\\')
        {
          cell->neighbours[0] = GetCell(x - 1, y - 1);
        }
        if (IsPositionInBounds(x, y - 1) &&
            lines[inputRow - 1][inputColumn - (inputRow % 2)] == '|')
        {
          cell->neighbours[1] = GetCell(x, y - 1);
        }
        if (IsPositionInBounds(x + 1, y - 1) &&
            lines[inputRow - 1][inputColumn + 1 - (inputRow % 2)] == '/')
        {
          cell->neighbours[2] = GetCell(x + 1, y - 1);
        }
        if (IsPositionInBounds(x - 1, y) && lines[inputRow][inputColumn - 1] == '-')
        {
          cell->neighbours[3] = GetCell(x - 1, y);
        }
        if (IsPositionInBounds(x + 1, y) && lines[inputRow][inputColumn + 1] == '-')
        {
          cell->neighbours[4] = GetCell(x + 1, y);
        }
        if (IsPositionInBounds(x - 1, y + 1) &&
            lines[inputRow + 1][inputColumn - 1 - (inputRow % 2)] == '/')
        {
          cell->neighbours[5] = GetCell(x - 1, y + 1);
        }
        if (IsPositionInBounds(x, y + 1) &&
            lines[inputRow + 1][inputColumn - (inputRow % 2)] == '|')
        {
          cell->neighbours[6] = GetCell(x, y + 1);
        }
        if (IsPositionInBounds(x + 1, y + 1) &&
            lines[inputRow + 1][inputColumn + 1 - (inputRow % 2)] == '\\')
        {
          cell->neighbours[7] = GetCell(x + 1, y + 1);
        }
      }
    }
  }

  // only setup neighbour connections once
  bFirst_parse = false;

  // Output current board after successful parse
  Print();
}

void Board::Print()
{
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    if (y == 0)
    {
      std::cerr << "\r\n  0 1 2 3 4 5 6 7 8" << std::endl;
    }
    else if (y % 2 == 0)
    {
      std::cerr << "  |/|\\|/|\\|/|\\|/|\\|" << std::endl;
    }
    else
    {
      std::cerr << "  |\\|/|\\|/|\\|/|\\|/|" << std::endl;
    }

    std::cerr << y << " ";
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      EState state = m_cells[y][x]->state;
      if (state == EState::WHITE)
      {
        std::cerr << "O ";
      }
      else if (state == EState::BLACK)
      {
        std::cerr << "# ";
      }
      else
      {
        std::cerr << ". ";
      }
    }
    std::cerr << std::endl;
  }
}

std::shared_ptr<Node> Board::GetCell(int x, int y)
{
  if (!IsPositionInBounds(x, y))
  {
    return nullptr;
  }
  return m_cells[y][x];
}

bool Board::IsPositionInBounds(int x, int y)
{
  return x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT;
}

std::string Board::GetPosition(EMove move)
{
  std::string input;
  if (m_mode == EMode::HUMAN)
  {
    // Output current board
    Print();

    // Output turn information
    auto turns = FindTurns(EState::WHITE);
    std::cerr << turns.size() << " available Turns: \n";
    for (auto& turn : turns)
    {
      std::cerr << turn->ToString() << "\n";
    }

    // Check what the server is asking of us and output an appropriate message
    if (move == EMove::ORIGIN_X)
    {
      std::cout << "Origin X: ";
    }
    else if (move == EMove::ORIGIN_Y)
    {
      std::cout << "Origin Y: ";
    }
    else if (move == EMove::DEST_X)
    {
      std::cout << "Destination X: ";
    }
    else if (move == EMove::DEST_Y)
    {
      std::cout << "Destination Y: ";
    }
    else if (move == EMove::W_OR_A)
    {
      std::cout << "Withdraw or Approach?: ";
    }

    // std::cin >> input; // does not parse whitespaces!
    std::getline(std::cin, input);
  }
  else
  {
    input = m_player.GetNextMove(shared_from_this(), move);
  }
  return input;
}

const std::list<std::shared_ptr<Turn>> Board::FindTurnsForNode(EState movingState,
                                                               std::shared_ptr<Node> node,
                                                               std::shared_ptr<Turn> previousTurn)
{
  std::list<std::shared_ptr<Turn>> capturingTurns;
  std::list<std::shared_ptr<Turn>> paikaTurns;

  // if we find a stone of my color
  if (node->state == movingState)
  {
    // iterate over neighbors
    for (int i = 0; i < 8; i++)
    {
      auto neighbour = node->neighbours[i];

      // if the neighbor node is empty and the previous turn doesn't forbid this direction via the
      // rules
      if ((neighbour != nullptr && neighbour->state == EState::EMPTY) &&
          (previousTurn == nullptr ||
           (!previousTurn->NodeAlreadyVisited(neighbour) && previousTurn->IsNewDirection(i))))
      {
        // this is a possible turn!
        auto move            = std::make_shared<Move>(node, i);
        auto captureForward  = std::make_shared<Capture>(GetCapturesInDirection(*move, false));
        auto captureBackward = std::make_shared<Capture>(GetCapturesInDirection(*move, true));

        // FIXME: Duplication for forward and backward
        auto forwardTurn          = std::make_shared<Turn>(move, captureForward);
        forwardTurn->previousTurn = previousTurn;
        if (captureForward->capturedNodes.size() > 0)
        {
          std::list<std::shared_ptr<Turn>> forwardTurns =
            GenerateTurnsWithFollowingTurns(forwardTurn, movingState);
          for (auto turn : forwardTurns)
          {
            capturingTurns.emplace_back(turn);
          }
        }
        else
        {
          paikaTurns.emplace_back(forwardTurn);
        }

        auto backwardTurn          = std::make_shared<Turn>(move, captureBackward);
        backwardTurn->previousTurn = previousTurn;
        if (captureBackward->capturedNodes.size() > 0)
        {
          std::list<std::shared_ptr<Turn>> backwardTurns =
            GenerateTurnsWithFollowingTurns(backwardTurn, movingState);
          for (auto turn : backwardTurns)
          {
            capturingTurns.emplace_back(turn);
          }
        }
      }
    }
  }
  return capturingTurns.size() > 0 ? capturingTurns : paikaTurns;
}

const std::list<std::shared_ptr<Turn>> Board::GenerateTurnsWithFollowingTurns(
  std::shared_ptr<Turn> startTurn,
  EState pieceColor)
{
  std::list<std::shared_ptr<Turn>> generatedTurns;
  ApplyTurn(startTurn);

  std::list<std::shared_ptr<Turn>> turns =
    FindTurnsForNode(pieceColor, startTurn->move->To(), startTurn);
  if (turns.size() > 0)
  {
    for (auto turn : turns)
    {
      auto newTurn          = std::make_shared<Turn>(startTurn->move, startTurn->capture);
      newTurn->previousTurn = startTurn->previousTurn;
      newTurn->nextTurn     = turn;
      turn->previousTurn    = newTurn;
      generatedTurns.emplace_back(newTurn);
    }
  }
  else
  {
    generatedTurns.emplace_back(startTurn);
  }
  RollbackTurn(startTurn);
  return generatedTurns;
}

const std::list<std::shared_ptr<Turn>> Board::FindTurns(EState movingState)
{
  std::list<std::shared_ptr<Turn>> captureTurns;
  std::list<std::shared_ptr<Turn>> paikaTurns;

  // iterate over board
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      std::shared_ptr<Node> node = GetCell(x, y);
      auto potential_turns       = FindTurnsForNode(movingState, node, nullptr);

      // Separate capturing from paika turns
      // TODO: This check happens twice, here and in FindTurnsForNode (which returns either only
      // capturing or only paika). We could potentially increase performance by only checking once
      // and e.g. returning a flag for capturing or paika in FindTurnsForNode.
      for (const auto& turn : potential_turns)
      {
        if (turn->capture->capturedNodes.size() > 0)
        {
          captureTurns.emplace_back(turn); // TODO: Consider moving (splice)
        }
        else
        {
          paikaTurns.emplace_back(turn); // TODO: Consider moving (splice)
        }
      }
    }
  }

  return captureTurns.size() > 0 ? captureTurns : paikaTurns;
}

void Board::ApplyTurn(std::shared_ptr<Turn> turn)
{
  // Remove captured
  for (auto& node : turn->capture->capturedNodes)
  {
    node->state = EState::EMPTY;
  }

  // Make turn
  std::swap(turn->move->To()->state, turn->move->From()->state);
}

void Board::RollbackTurn(std::shared_ptr<Turn> turn)
{
  // Reset turn
  std::swap(turn->move->To()->state, turn->move->From()->state);

  // Put captured back
  for (auto& node : turn->capture->capturedNodes)
  {
    // FIXME: Can we avoid this if-check? We just need the opposite color
    // SUGGESTION: node->state = (EState)(2 * (int)(turn->move->From()->state) % 3);
    if (turn->move->From()->state == EState::BLACK)
    {
      node->state = EState::WHITE;
    }
    else
    {
      node->state = EState::BLACK;
    }
  }
}

void Board::ApplyTurnWithFollowing(std::shared_ptr<Turn> turn)
{
  // Apply in order
  std::shared_ptr<Turn> currentTurn = turn;

  while (currentTurn != nullptr)
  {
    ApplyTurn(turn);
    currentTurn = currentTurn->nextTurn;
  }
}

void Board::RollbackTurnWithFollowing(std::shared_ptr<Turn> turn)
{
  // Apply in reverse order
  // First, get the last turn
  std::shared_ptr<Turn> lastTurn = turn;

  while (lastTurn->nextTurn != nullptr)
  {
    lastTurn = lastTurn->nextTurn;
  }

  // Rollback in order
  std::shared_ptr<Turn> currentTurn = lastTurn;

  while (currentTurn != nullptr)
  {
    RollbackTurn(turn);
    currentTurn = currentTurn->previousTurn;
  }
}

const std::vector<std::shared_ptr<Node>> Board::GetCapturesInDirection(const Move& move,
                                                                       bool reverse_direction)
{
  std::vector<std::shared_ptr<Node>> captures;

  EState myState = move.From()->state;

  int direction;
  std::shared_ptr<Node> currentNeighbour;

  if (reverse_direction)
  {
    direction        = 7 - move.direction;
    currentNeighbour = move.From()->neighbours[direction];
  }
  else
  {
    direction        = move.direction;
    currentNeighbour = move.To()->neighbours[direction];
  }

  // if there is a field, and on that field is a stone of the other color count up
  while (currentNeighbour != nullptr && currentNeighbour->state != EState::EMPTY &&
         currentNeighbour->state != myState)
  {
    captures.emplace_back(currentNeighbour);

    // continue with next neighbor
    currentNeighbour = currentNeighbour->neighbours[direction];
  }

  return captures;
}

std::string Move::ToString() const
{
  return "From " + From()->ToString() + " to " + To()->ToString();
}
