#include <iostream>
#include <limits.h>
#include <memory>
#include <sstream>

#include "Board.h"

// ////////////////////////////////////////////////////////////////////////
// Board (Constructor)
//

Board::Board(EMode eMode, int nDepth) : m_eMode(eMode)
{
  // iterate over board, creating cells
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      m_pCells[y][x] = std::make_shared<Node>();
      if (x > 0 && x < BOARD_WIDTH - 1 && y > 0 && y < BOARD_HEIGHT)
      {
        m_pCells[y][x]->m_bIsMiddleField = true;
      }
    }
  }

  // setting search depth for ai player
  m_aiPlayer.SetDepth(nDepth);
}

// ////////////////////////////////////////////////////////////////////////
// Parse
//
// Parameter: string of board content received from the server
// Method parses this string and creates board content from it

void Board::Parse(std::string strBoardContent)
{
  static bool bFirstParse = true; // using static variable to determine first parsing phase
  std::istringstream stream(strBoardContent);
  std::string str;

  std::vector<std::string> vecLines;
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
      vecLines.push_back(str);
    }
  }

  for (int y = 0;; y++)
  {
    int nInputRow = y * 2 + 1;
    if (nInputRow >= (int)vecLines.size())
    {
      break;
    }

    auto strLine  = vecLines[nInputRow];
    int nLineSize = (int)strLine.size();
    for (int x = 0; x < nLineSize; x++)
    {
      if (!IsPositionInBounds(x, y))
      {
        break;
      }

      int nInputColumn = x * 2 + (nInputRow % 2);
      char cCharacter  = strLine[nInputColumn];

      std::shared_ptr<Node> pCell = GetCell(x, y);
      pCell->x                    = x;
      pCell->y                    = y;

      // TODO: define "colors" as constants, add property playerColor
      if (cCharacter == '1' || cCharacter == '#')
      {
        pCell->eState = EState::BLACK;
      }
      else if (cCharacter == '2' || cCharacter == 'O')
      {
        pCell->eState = EState::WHITE;
      }
      else
      {
        pCell->eState = EState::EMPTY;
      }

      if (bFirstParse)
      {
        if (IsPositionInBounds(x - 1, y - 1) &&
            vecLines[nInputRow - 1][nInputColumn - 1 - (nInputRow % 2)] == '\\')
        {
          pCell->m_bIsDiagonalField = true;
          pCell->m_aNeighbours[0]   = GetCell(x - 1, y - 1);
        }
        if (IsPositionInBounds(x, y - 1) &&
            vecLines[nInputRow - 1][nInputColumn - (nInputRow % 2)] == '|')
        {
          pCell->m_aNeighbours[1] = GetCell(x, y - 1);
        }
        if (IsPositionInBounds(x + 1, y - 1) &&
            vecLines[nInputRow - 1][nInputColumn + 1 - (nInputRow % 2)] == '/')
        {
          pCell->m_bIsDiagonalField = true;
          pCell->m_aNeighbours[2]   = GetCell(x + 1, y - 1);
        }
        if (IsPositionInBounds(x - 1, y) && vecLines[nInputRow][nInputColumn - 1] == '-')
        {
          pCell->m_aNeighbours[3] = GetCell(x - 1, y);
        }
        if (IsPositionInBounds(x + 1, y) && vecLines[nInputRow][nInputColumn + 1] == '-')
        {
          pCell->m_aNeighbours[4] = GetCell(x + 1, y);
        }
        if (IsPositionInBounds(x - 1, y + 1) &&
            vecLines[nInputRow + 1][nInputColumn - 1 - (nInputRow % 2)] == '/')
        {
          pCell->m_bIsDiagonalField = true;
          pCell->m_aNeighbours[5]   = GetCell(x - 1, y + 1);
        }
        if (IsPositionInBounds(x, y + 1) &&
            vecLines[nInputRow + 1][nInputColumn - (nInputRow % 2)] == '|')
        {
          pCell->m_aNeighbours[6] = GetCell(x, y + 1);
        }
        if (IsPositionInBounds(x + 1, y + 1) &&
            vecLines[nInputRow + 1][nInputColumn + 1 - (nInputRow % 2)] == '\\')
        {
          pCell->m_bIsDiagonalField = true;
          pCell->m_aNeighbours[7]   = GetCell(x + 1, y + 1);
        }
      }
    }
  }
  // only setup neighbour connections once
  bFirstParse = false;

#ifndef OMIT_OUTPUT
  // Output current board after successful parse
  Print();
#endif
}

// ////////////////////////////////////////////////////////////////////////
// Print
//
// Prints the board

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
      EState eState = m_pCells[y][x]->eState;
      if (eState == EState::WHITE)
      {
        std::cerr << "O ";
      }
      else if (eState == EState::BLACK)
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

// ////////////////////////////////////////////////////////////////////////
// GetPosition
//
// Return:  the String which the Client should send to the Server next

std::string Board::GetPosition(EMove eMove)
{
  std::string strInput;
  if (m_eMode == EMode::HUMAN)
  {
    // Output current board
    Print();

    // Output turn information
    auto pTurns = FindTurns(EState::WHITE);
    std::cerr << pTurns.size() << " available Turns: \n";
    for (auto& turn : pTurns)
    {
      std::cerr << turn->ToString() << "\n";
    }

    // Check what the server is asking of us and output an appropriate message
    if (eMove == EMove::ORIGIN_X)
    {
      std::cout << "Origin X: ";
    }
    else if (eMove == EMove::ORIGIN_Y)
    {
      std::cout << "Origin Y: ";
    }
    else if (eMove == EMove::DEST_X)
    {
      std::cout << "Destination X: ";
    }
    else if (eMove == EMove::DEST_Y)
    {
      std::cout << "Destination Y: ";
    }
    else if (eMove == EMove::W_OR_A)
    {
      std::cout << "Withdraw or Approach?: ";
    }

    // std::cin >> strInput; // does not parse whitespaces!
    std::getline(std::cin, strInput);
  }
  else
  {
    strInput = m_aiPlayer.GetNextMove(*this, eMove);
  }
  return strInput;
}

// ////////////////////////////////////////////////////////////////////////
// FindTurnsForNode
//
// Return:  a list of all possible turns which the given Node could take

const std::list<std::shared_ptr<Turn>> Board::FindTurnsForNode(EState eMovingState,
                                                               std::shared_ptr<Node> pNode,
                                                               std::shared_ptr<Turn> pPreviousTurn,
                                                               bool bIncludePaika)
{
  std::list<std::shared_ptr<Turn>> listCapturingTurns;
  std::list<std::shared_ptr<Turn>> listPaikaTurns;

  // if we find a stone of my color
  if (pNode->eState == eMovingState)
  {
    // iterate over neighbors
    for (int i = 0; i < 8; i++)
    {
      auto pNeighbour = pNode->m_aNeighbours[i];

      // if the neighbor node is empty and the previous turn doesn't forbid this direction via the
      // rules
      if ((pNeighbour != nullptr && pNeighbour->eState == EState::EMPTY) &&
          (pPreviousTurn == nullptr ||
           (!pPreviousTurn->NodeAlreadyVisited(pNeighbour) && pPreviousTurn->IsNewDirection(i))))
      {
        // this is a possible turn!
        auto pMove            = std::make_shared<Move>(pNode, i);
        auto pCaptureForward  = std::make_shared<Capture>(GetCapturesInDirection(*pMove, false));
        auto pCaptureBackward = std::make_shared<Capture>(GetCapturesInDirection(*pMove, true));

        // FIXME: Duplication for forward and backward
        auto pForwardTurn             = std::make_shared<Turn>(pMove, pCaptureForward);
        pForwardTurn->m_pPreviousTurn = pPreviousTurn;
        if (pCaptureForward->m_vecCaptureNodes.size() > 0)
        {
          std::list<std::shared_ptr<Turn>> listForwardTurns =
            GenerateTurnsWithFollowingTurns(pForwardTurn, eMovingState);
          for (auto pTurn : listForwardTurns)
          {
            listCapturingTurns.emplace_back(pTurn);
          }
        }
        else
        {
          listPaikaTurns.emplace_back(pForwardTurn);
        }

        auto pBackwardTurn             = std::make_shared<Turn>(pMove, pCaptureBackward);
        pBackwardTurn->m_pPreviousTurn = pPreviousTurn;
        if (pCaptureBackward->m_vecCaptureNodes.size() > 0)
        {
          std::list<std::shared_ptr<Turn>> listbackwardTurns =
            GenerateTurnsWithFollowingTurns(pBackwardTurn, eMovingState);
          for (auto pTurn : listbackwardTurns)
          {
            listCapturingTurns.emplace_back(pTurn);
          }
        }
      }
    }
  }

  if (bIncludePaika)
  {
    return listCapturingTurns.size() > 0 ? listCapturingTurns : listPaikaTurns;
  }
  else
  {
    return listCapturingTurns;
  }
}

// ////////////////////////////////////////////////////////////////////////
// GenerateTurnsWithFollowingTurns
//
// Generates all combination of valid follow up turns for a given turn

const std::list<std::shared_ptr<Turn>> Board::GenerateTurnsWithFollowingTurns(
  std::shared_ptr<Turn> pStartTurn,
  EState ePieceColor)
{
  std::list<std::shared_ptr<Turn>> listGeneratedTurns;
  ApplyTurn(pStartTurn);

  std::list<std::shared_ptr<Turn>> listTurns =
    FindTurnsForNode(ePieceColor, pStartTurn->m_pMove->To(), pStartTurn, false);
  if (listTurns.size() > 0)
  {
    for (auto pTurn : listTurns)
    {
      auto pNewTurn = std::make_shared<Turn>(pStartTurn->m_pMove, pStartTurn->m_pCapture);
      pNewTurn->m_pPreviousTurn = pStartTurn->m_pPreviousTurn;
      pNewTurn->m_pNextTurn     = pTurn;
      pTurn->m_pPreviousTurn    = pNewTurn;
      listGeneratedTurns.emplace_back(pNewTurn);
    }
  }
  else
  {
    listGeneratedTurns.emplace_back(pStartTurn);
  }
  RollbackTurn(pStartTurn);
  return listGeneratedTurns;
}

// ////////////////////////////////////////////////////////////////////////
// FindTurns
//
// Return:  a list of all possible turns which any Node on the field with
//          the given state could take.

const std::list<std::shared_ptr<Turn>> Board::FindTurns(EState eMovingState)
{
  std::list<std::shared_ptr<Turn>> listCaptureTurns;
  std::list<std::shared_ptr<Turn>> listPaikaTurns;

  // iterate over board
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      std::shared_ptr<Node> pNode = GetCell(x, y);
      auto listPotentialTurns     = FindTurnsForNode(eMovingState, pNode, nullptr, true);

      // Separate capturing from paika turns
      // TODO: This check happens twice, here and in FindTurnsForNode (which returns either only
      // capturing or only paika). We could potentially increase performance by only checking once
      // and e.g. returning a flag for capturing or paika in FindTurnsForNode.
      for (const auto& pTurn : listPotentialTurns)
      {
        if (pTurn->m_pCapture->m_vecCaptureNodes.size() > 0)
        {
          listCaptureTurns.emplace_back(pTurn); // TODO: Consider moving (splice)
        }
        else
        {
          listPaikaTurns.emplace_back(pTurn); // TODO: Consider moving (splice)
        }
      }
    }
  }

  return listCaptureTurns.size() > 0 ? listCaptureTurns : listPaikaTurns;
}

// ////////////////////////////////////////////////////////////////////////
// ApplyTurn
//
// Apply a turn to the Board by moving the corresponding Node and removing Captures.

void Board::ApplyTurn(std::shared_ptr<Turn> pTurn)
{
  // Remove captured
  for (auto& pNode : pTurn->m_pCapture->m_vecCaptureNodes)
  {
    pNode->eState = EState::EMPTY;
  }

  // Make turn
  std::swap(pTurn->m_pMove->To()->eState, pTurn->m_pMove->From()->eState);
}

// ////////////////////////////////////////////////////////////////////////
// RollbackTurn
//
// Rollback a turn which was previously applied with ApplyTurn.

void Board::RollbackTurn(std::shared_ptr<Turn> pTurn)
{
  // Reset turn
  std::swap(pTurn->m_pMove->To()->eState, pTurn->m_pMove->From()->eState);

  // Put captured back
  for (auto& pNode : pTurn->m_pCapture->m_vecCaptureNodes)
  {
    // FIXME: Can we avoid this if-check? We just need the opposite color
    // SUGGESTION: node->eState = (EState)(2 * (int)(turn->move->From()->eState) % 3);
    if (pTurn->m_pMove->From()->eState == EState::BLACK)
    {
      pNode->eState = EState::WHITE;
    }
    else
    {
      pNode->eState = EState::BLACK;
    }
  }
}

// ////////////////////////////////////////////////////////////////////////
// ApplyTurnWithFollowing
//
// Apply a turn to the Board including its follow up turns.

void Board::ApplyTurnWithFollowing(std::shared_ptr<Turn> pTurn)
{
  // Apply in order
  std::shared_ptr<Turn> pCurrentTurn = pTurn;

  while (pCurrentTurn != nullptr)
  {
    ApplyTurn(pTurn);
    pCurrentTurn = pCurrentTurn->m_pNextTurn;
  }
}

// ////////////////////////////////////////////////////////////////////////
// RollbackTurnWithFollowing
//
// Rollback a turn including its follow up turns which was previously applied with ApplyTurn.

void Board::RollbackTurnWithFollowing(std::shared_ptr<Turn> pTurn)
{
  // Apply in reverse order
  // First, get the last turn
  std::shared_ptr<Turn> pLastTurn = pTurn;

  while (pLastTurn->m_pNextTurn != nullptr)
  {
    pLastTurn = pLastTurn->m_pNextTurn;
  }

  // Rollback in order
  std::shared_ptr<Turn> pCurrentTurn = pLastTurn;

  while (pCurrentTurn != nullptr)
  {
    RollbackTurn(pTurn);
    pCurrentTurn = pCurrentTurn->m_pPreviousTurn;
  }
}

// ////////////////////////////////////////////////////////////////////////
// GetCell
//

std::shared_ptr<Node> Board::GetCell(int x, int y)
{
  if (!IsPositionInBounds(x, y))
  {
    return nullptr;
  }
  return m_pCells[y][x];
}

// ////////////////////////////////////////////////////////////////////////
// IsPositionInBounds
//

bool Board::IsPositionInBounds(int x, int y)
{
  return x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT;
}

// ////////////////////////////////////////////////////////////////////////
// DistanceToNearestEnemy
//
// Return: the distance to the nearest enemy, neede for some heuristics

int Board::DistanceToNearestEnemy(std::shared_ptr<Node> pNode)
{
  if (pNode->eState == EState::EMPTY)
  {
    return -1;
  }
  EState eMyState  = pNode->eState;
  int nMinDistance = INT_MAX;
  for (int y = 0; y < BOARD_HEIGHT; y++)
  {
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
      std::shared_ptr<Node> pCurrentNode = GetCell(x, y);
      if (pCurrentNode->eState != eMyState && pCurrentNode->eState != EState::EMPTY)
      {
        int nDistance = DistanceBetweenNodes(pNode, pCurrentNode);
        nMinDistance  = nMinDistance > nDistance ? nDistance : nMinDistance;
      }
    }
  }
  return nMinDistance == INT_MAX ? -1 : nMinDistance;
}

// ////////////////////////////////////////////////////////////////////////
// DistanceBetweenNodes
//
// Return: the distance between two nodes using valid movement on the board

int Board::DistanceBetweenNodes(std::shared_ptr<Node> pStart, std::shared_ptr<Node> pEnd)
{
  int nYCur     = pStart->y;
  int nXCur     = pStart->x;
  int nYEnd     = pEnd->y;
  int nXEnd     = pEnd->x;
  int nYDir     = nYEnd > nYCur ? 1 : -1;
  int nXDir     = nXEnd > nXCur ? 1 : -1;
  int ndistance = 0;
  while (nYCur != nYEnd || nXCur != nXEnd)
  {
    auto pCurrentNode = GetCell(nXCur, nYCur);
    if (pCurrentNode->m_bIsDiagonalField && nYCur != nYEnd && nXCur != nXEnd)
    {
      nXCur += nXDir;
      nYCur += nYDir;
    }
    else
    {
      if (nYCur != nYEnd)
      {
        nYCur += nYDir;
      }
      else
      {
        nXCur += nXDir;
      }
    }
    ndistance++;
  }
  return ndistance;
}

// ////////////////////////////////////////////////////////////////////////
// GetCapturesInDirection
//
// return: all pieces captured by a move, either in the direction of the move or behind the
// move.

const std::vector<std::shared_ptr<Node>> Board::GetCapturesInDirection(const Move& move,
                                                                       bool bReverseDirection)
{
  std::vector<std::shared_ptr<Node>> vecCaptures;

  EState eMyState = move.From()->eState;

  int nDirection;
  std::shared_ptr<Node> pCurrentNeighbour;

  if (bReverseDirection)
  {
    nDirection        = 7 - move.m_nDirection;
    pCurrentNeighbour = move.From()->m_aNeighbours[nDirection];
  }
  else
  {
    nDirection        = move.m_nDirection;
    pCurrentNeighbour = move.To()->m_aNeighbours[nDirection];
  }

  // if there is a field, and on that field is a stone of the other color count up
  while (pCurrentNeighbour != nullptr && pCurrentNeighbour->eState != EState::EMPTY &&
         pCurrentNeighbour->eState != eMyState)
  {
    vecCaptures.emplace_back(pCurrentNeighbour);

    // continue with next neighbor
    pCurrentNeighbour = pCurrentNeighbour->m_aNeighbours[nDirection];
  }

  return vecCaptures;
}

// ////////////////////////////////////////////////////////////////////////
// GetTurnsPlayed
//
// Return: how many turns the AIPlayer has played

int Board::GetTurnsPlayed()
{
  return m_aiPlayer.GetTurnsPlayed();
}