#include <iostream>
#include <sstream>

#include "Board.h"

std::string Node::ToString() const
{
  return std::to_string (x) + " " + std::to_string (y);
}

Node* Move::From() const
{
  return m_origin;
}

Node* Move::To() const
{
  return m_origin->neighbours[direction];
}

std::string Capture::ToString() const
{
  return "Capturing " + std::to_string (capturedNodes.size()) + " nodes";
}

std::string Turn::ToString() const
{
  std::string ret = move->ToString() + " " + capture->ToString();

  if (nextTurn != nullptr)
  {
    ret += " with " + std::to_string (GetTurnChainLength()) + " following turns";
  }

  return ret;
}

uint Turn::GetTurnChainLength() const
{
  const Turn* current_turn = this;
  uint length              = 0;

  while (current_turn != nullptr)
  {
    current_turn = current_turn->nextTurn;
    length++;
  }

  return length;
}

bool Turn::IsWithdraw() const
{
  // The turn is a withdraw if the first captured node is a neighbor of From.
  Node* first_capture = capture->capturedNodes.front();

  for (const auto& neighbour : move->From()->neighbours)
  {
    if (neighbour == first_capture)
    {
      return true;
    }
  }

  return false;
}

Board::Board (EMode mode) : m_mode (mode)
{
}

Board::~Board()
{
}

void Board::Parse (std::string boardContent)
{
  std::istringstream stream (boardContent);
  std::string str;

  std::vector<std::string> lines;
  while (getline (stream, str))
  {
    // Remove carriage return if a server with Windows line endings is used
    if (str[0] == '\r' && str.length() > 0)
    {
      str = str.substr (1);
    }

    // don't add empty lines
    if (str.length() != 0)
    {
      lines.push_back (str);
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
      if (!IsPositionInBounds (x, y))
      {
        break;
      }

      // int inputColumn = x * 2 + 2;
      int inputColumn = x * 2 + (inputRow % 2);
      char character  = line[inputColumn];

      auto cell = GetCell (x, y);

      // TODO: Assign this earlier?
      cell->x = x;
      cell->y = y;

      // TODO: dont override valid current stone selection
      // if (movingPiece && movingPiece->x == x && movingPiece->y == y)

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

      // TODO: only setup neighbour connections during first parsing phase @rene
      if (IsPositionInBounds (x - 1, y - 1) &&
          lines[inputRow - 1][inputColumn - 1 - (inputRow % 2)] == '\\')
      {
        cell->neighbours[0] = GetCell (x - 1, y - 1);
      }
      if (IsPositionInBounds (x, y - 1) && lines[inputRow - 1][inputColumn - (inputRow % 2)] == '|')
      {
        cell->neighbours[1] = GetCell (x, y - 1);
      }
      if (IsPositionInBounds (x + 1, y - 1) &&
          lines[inputRow - 1][inputColumn + 1 - (inputRow % 2)] == '/')
      {
        cell->neighbours[2] = GetCell (x + 1, y - 1);
      }
      if (IsPositionInBounds (x - 1, y) && lines[inputRow][inputColumn - 1] == '-')
      {
        cell->neighbours[3] = GetCell (x - 1, y);
      }
      if (IsPositionInBounds (x + 1, y) && lines[inputRow][inputColumn + 1] == '-')
      {
        cell->neighbours[4] = GetCell (x + 1, y);
      }
      if (IsPositionInBounds (x - 1, y + 1) &&
          lines[inputRow + 1][inputColumn - 1 - (inputRow % 2)] == '/')
      {
        cell->neighbours[5] = GetCell (x - 1, y + 1);
      }
      if (IsPositionInBounds (x, y + 1) && lines[inputRow + 1][inputColumn - (inputRow % 2)] == '|')
      {
        cell->neighbours[6] = GetCell (x, y + 1);
      }
      if (IsPositionInBounds (x + 1, y + 1) &&
          lines[inputRow + 1][inputColumn + 1 - (inputRow % 2)] == '\\')
      {
        cell->neighbours[7] = GetCell (x + 1, y + 1);
      }
    }
  }

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

Node* Board::GetCell (int x, int y)
{
  if (!IsPositionInBounds (x, y))
  {
    return nullptr;
  }
  return &m_cells[y][x];
}

bool Board::IsPositionInBounds (int x, int y)
{
  return x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT;
}

// temp implementation for stdin input by user: TODO add logic implementation here
std::string Board::GetPosition (EMove move)
{
  // TODO: own implementation for human, AI player
  std::string input;
  if (m_mode == EMode::HUMAN)
  {
    // Output turn information
    auto turns = FindTurns (EState::WHITE);
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
    std::getline (std::cin, input);
  }
  else
  {
    Print();

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
        // We're done with the previous chain, get a new optimal turn
        auto turns = FindTurns (EState::WHITE);

        // Get the optimal turn
        // TODO: Minimax
        uint optimal_value = 0;

        // For now, the optimal turn is the one with the most captured + following turns
        for (const auto& turn : turns)
        {
          uint score = turn->capture->capturedNodes.size() + turn->GetTurnChainLength();

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
      input = std::to_string (m_turn_to_handle->move->From()->x);
    }
    else if (move == EMove::ORIGIN_Y)
    {
      input = std::to_string (m_turn_to_handle->move->From()->y);
    }
    else if (move == EMove::DEST_X)
    {
      input = std::to_string (m_turn_to_handle->move->To()->x);
    }
    else if (move == EMove::DEST_Y)
    {
      input              = std::to_string (m_turn_to_handle->move->To()->y);
      m_potentially_done = true;
    }
    else if (move == EMove::W_OR_A)
    {
      if (m_turn_to_handle->IsWithdraw())
      {
        input = "W";
      }
      else
      {
        input = "A";
      }
      m_potentially_done = true;
    }
  }
  return input;
}

const std::list<Turn*> Board::FindTurnsForNode (EState movingState, Node* node)
{
  std::list<Turn*> turns;

  // if we find a stone of my color
  if (node->state == movingState)
  {
    // iterate over neighbors
    for (int i = 0; i < 8; i++)
    {
      auto neighbour = node->neighbours[i];

      // if neighbor node is empty
      if (neighbour != nullptr && neighbour->state == EState::EMPTY)
      {
        // this is a possible turn!
        Move* move               = new Move (node, i);
        Capture* captureForward  = new Capture (GetCapturesInDirection (*move, false));
        Capture* captureBackward = new Capture (GetCapturesInDirection (*move, true));

        // are there following turns?
        Turn* forwardTurn = new Turn (move, captureForward);
        if (captureForward->capturedNodes.size() > 0)
        {
          ApplyTurn (forwardTurn);

          std::list<Turn*> turns = FindTurnsForNode (movingState, neighbour);
          if (turns.size() > 0)
          {
            forwardTurn->nextTurn = turns.front();
          }

          RollbackTurn (forwardTurn);
        }

        Turn* backwardTurn = new Turn (move, captureBackward);
        if (captureBackward->capturedNodes.size() > 0)
        {
          ApplyTurn (backwardTurn);

          std::list<Turn*> turns = FindTurnsForNode (movingState, neighbour);
          if (turns.size() > 0)
          {
            backwardTurn->nextTurn = turns.front();
          }

          RollbackTurn (backwardTurn);
        }

        turns.emplace_back (forwardTurn);
        turns.emplace_back (backwardTurn);
      }
    }
  }

  return turns;
}

const std::list<Turn*> Board::FindTurns (EState movingState)
{
  std::list<Turn*> turns;

  // iterate over board
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      Node* node = GetCell (x, y);
      auto moves = FindTurnsForNode (movingState, node);
      turns.splice (turns.end(), moves);
    }
  }

  return turns;
}

void Board::ApplyTurn (Turn* turn)
{
  // Remove captured
  for (auto& node : turn->capture->capturedNodes)
  {
    node->state = EState::EMPTY;
  }

  // Make turn
  std::swap (turn->move->To()->state, turn->move->From()->state);
}

void Board::RollbackTurn (Turn* turn)
{
  // Reset turn
  std::swap (turn->move->To()->state, turn->move->From()->state);

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

const std::vector<Node*> Board::GetCapturesInDirection (const Move& move, bool reverse_direction)
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
    captures.emplace_back (currentNeighbour);

    // continue with next neighbor
    currentNeighbour = currentNeighbour->neighbours[direction];
  }

  return captures;
}

const std::vector<Node*> Board::GetBestCaptures (const Move& move)
{
  auto captures_forwards  = GetCapturesInDirection (move, false);
  auto captures_backwards = GetCapturesInDirection (move, true);

  return captures_forwards.size() > captures_backwards.size() ? captures_forwards
                                                              : captures_backwards;
}

std::string Move::ToString() const
{
  return "From " + From()->ToString() + " to " + To()->ToString();
}
