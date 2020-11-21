#include <iostream>
#include <limits.h>
#include <sstream>

#include "Board.h"

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

      Node* cell = GetCell(x, y);
      cell->x    = x;
      cell->y    = y;

      // TODO: define "colors" as constants, add property playerColor
      if (character == '1' || character == '#')
      {
        cell->state = EState::BLACK;
      }
      else if (character == '2' || character == 'O')
      {
        cell->state = EState::WHITE;
      }
      else if (character == '*') // current cell -> needed?
      {
        cell->state = EState::CURRENT;
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

  bFirst_parse = false;

  // print board for human player after successful parse
  if (m_mode == EMode::HUMAN)
  {
    Print();
  }
}

void Board::Print()
{
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    if (y % 2 == 0)
    {
      if (y == 0)
      {
        std::cout << "  0 1 2 3 4 5 6 7 8" << std::endl;
      }
      else
      {
        std::cout << "  |/|\\|/|\\|/|\\|/|\\|" << std::endl;
      }
    }
    else
    {
      std::cout << "  |\\|/|\\|/|\\|/|\\|/|" << std::endl;
    }

    std::cout << y << " ";
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      EState state = (&m_cells[y][x])->state;
      if (state == EState::WHITE)
      {
        std::cout << "O ";
      }
      else if (state == EState::BLACK)
      {
        std::cout << "# ";
      }
      else if (state == EState::CURRENT)
      {
        std::cout << "* ";
      }
      else
      {
        std::cout << ". ";
      }
    }
    std::cout << std::endl;
  }
}

Node* Board::GetCell(int x, int y)
{
  if (!IsPositionInBounds(x, y))
  {
    return nullptr;
  }
  return &m_cells[y][x];
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
    // Output turn information
    auto turns = FindTurns(EState::WHITE);
    std::cout << turns.size() << " available Turns: \n";
    for (auto turn : turns)
    {
      std::cout << turn->ToString() << "\n";
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
    Print();

    return m_player.GetNextMove(this, move);
  }
}

const std::list<Turn*> Board::FindTurnsForNode(EState movingState, Node* node, Turn* previousTurn)
{
  std::list<Turn*> capturingTurns;
  std::list<Turn*> paikaTurns;

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
        Move* move               = new Move(node, i);
        Capture* captureForward  = new Capture(GetCapturesInDirection(*move, false));
        Capture* captureBackward = new Capture(GetCapturesInDirection(*move, true));

        // FIXME: Duplication for forward and backward
        Turn* forwardTurn = new Turn(move, captureForward);
        if (captureForward->capturedNodes.size() > 0)
        {
          // If there are following turns: Apply the turn, recursively create the chain of
          // turns, and rollback
          ApplyTurn(forwardTurn);

          std::list<Turn*> turns = FindTurnsForNode(movingState, neighbour, forwardTurn);
          if (turns.size() > 0)
          {
            // TODO: Currently the first turn is arbitrarily picked. But this should follow the
            // same heuristic as GetPosition!
            forwardTurn->nextTurn       = turns.front();
            turns.front()->previousTurn = forwardTurn;
          }

          RollbackTurn(forwardTurn);
        }

        Turn* backwardTurn = new Turn(move, captureBackward);
        if (captureBackward->capturedNodes.size() > 0)
        {
          ApplyTurn(backwardTurn);

          std::list<Turn*> turns = FindTurnsForNode(movingState, neighbour, backwardTurn);
          if (turns.size() > 0)
          {
            backwardTurn->nextTurn      = turns.front();
            turns.front()->previousTurn = backwardTurn;
          }

          RollbackTurn(backwardTurn);
        }
        captureForward->capturedNodes.size() > 0 ? capturingTurns.emplace_back(forwardTurn)
                                                 : paikaTurns.emplace_back(forwardTurn);
        captureBackward->capturedNodes.size() > 0 ? capturingTurns.emplace_back(backwardTurn)
                                                  : paikaTurns.emplace_back(backwardTurn);
      }
    }
  }
  return capturingTurns.size() > 0 ? capturingTurns : paikaTurns;
}

const std::list<Turn*> Board::FindTurns(EState movingState)
{
  std::list<Turn*> captureTurns;
  std::list<Turn*> paikaTurns;

  // iterate over board
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      Node* node           = GetCell(x, y);
      auto potential_turns = FindTurnsForNode(movingState, node, nullptr);

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

void Board::ApplyTurn(Turn* turn)
{
  // Remove captured
  for (auto& node : turn->capture->capturedNodes)
  {
    node->state = EState::EMPTY;
  }

  // Make turn
  std::swap(turn->move->To()->state, turn->move->From()->state);
}

void Board::RollbackTurn(Turn* turn)
{
  // Reset turn
  std::swap(turn->move->To()->state, turn->move->From()->state);

  // Put captured back
  for (auto& node : turn->capture->capturedNodes)
  {
    // FIXME: Can we avoid this if-check? We just need the opposite color
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

const std::vector<Node*> Board::GetCapturesInDirection(const Move& move, bool reverse_direction)
{
  std::vector<Node*> captures;

  EState myState = move.From()->state;

  int direction;
  Node* currentNeighbour;

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
