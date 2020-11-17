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
  // std::string nextTurnString = nextTurn ? "\n" + nextTurn->ToString() : "\n";
  // return move->ToString() + "\n" + capture->ToString() + nextTurnString;
  return move->ToString() + " " + capture->ToString();
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
      std::cout << turn.ToString() << "\n";
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

    // FIXME: Needs to be updated for new format (or may not be needed at all)
    /*
    // select current moving piece
    if (move == EMove::STONE || move == EMove::LOCATION)
    {
      if (input.length() > 2)
      {
        Node* cell    = GetCell ((input[0] - '0'), (input[2] - '0'));
        cell->state   = EState::CURRENT;
        m_movingPiece = cell;
      }
    }
    */
  }
  else // TODO: check if valid mode or just assume correct value?
  {
    input = "TODO: AI finds best option";
  }
  return input;
}

const std::vector<Turn> Board::FindTurns (EState movingState)
{
  std::vector<Turn> turns;

  // iterate over board
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      auto cell = GetCell (x, y);

      // if we find a stone of my color
      if (cell->state == movingState)
      {
        // iterate over neighbors
        for (int i = 0; i < 8; i++)
        {
          auto neighbour = cell->neighbours[i];

          // if neighbor node is empty
          if (neighbour != nullptr && neighbour->state == EState::EMPTY)
          {
            // this is a possible turn!

            Move* move               = new Move (cell, i);
            Capture* captureForward  = new Capture (GetCapturesInDirection (*move, false));
            Capture* captureBackward = new Capture (GetCapturesInDirection (*move, true));

            turns.emplace_back (Turn (move, captureForward));
            turns.emplace_back (Turn (move, captureBackward));
          }
        }
      }
    }
  }

  return turns;
}

// returns movable pieces for desired color, and integer denoting in which direction it can move
// this means pieces can occur twice in the returned vector if they can move in two or more
// directions
const std::vector<Move> Board::FindMoves (EState movingState)
{
  // if we have no moving piece its a normal turn else we continue moving that piece
  return m_movingPiece != nullptr ? FindContinuingMoves() : FindFirstMoves (movingState);
}

const std::vector<Move> Board::FindFirstMoves (EState movingState)
{
  std::vector<Move> nonCapturingMoves;
  std::vector<Move> capturingMoves;

  // iterate over board
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      auto cell = GetCell (x, y);
      // if we find a stone of my color
      if (cell->state == movingState)
      {
        // iterate over neighbors
        for (int i = 0; i < 8; i++)
        {
          auto neighbour = cell->neighbours[i];
          // if neighbor node is empty
          if (neighbour != nullptr && neighbour->state == EState::EMPTY)
          {
            Move move (cell, i);
            // check if move captures and add to corresponding vector
            if (GetBestCaptures (move).size() >= 1)
            {
              capturingMoves.push_back (move);
            }
            else
            {
              nonCapturingMoves.push_back (move);
            }
          }
        }
      }
    }
  }
  // capture is mandatory thus only return those moves if available
  return capturingMoves.size() > 0 ? capturingMoves : nonCapturingMoves;
}

const std::vector<Move> Board::FindContinuingMoves()
{
  std::vector<Move> nonCapturingMoves;
  std::vector<Move> capturingMoves;

  // iterate over neighbors of current moving piece
  for (int i = 0; i < 8; i++)
  {
    auto neighbour = m_movingPiece->neighbours[i];

    // if neighbor is empty
    if (neighbour != nullptr && neighbour->state == EState::EMPTY)
    {
      Move move (m_movingPiece, i);
      // check for captures and add to corresponding vector
      if (GetBestCaptures (move).size() >= 1)
      {
        capturingMoves.push_back (move);
      }
      else
      {
        nonCapturingMoves.push_back (move);
      }
    }
  }
  // capture is mandatory thus only return those moves if available
  return capturingMoves.size() > 0 ? capturingMoves : nonCapturingMoves;
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
