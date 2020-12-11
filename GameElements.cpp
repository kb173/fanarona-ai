#include "GameElements.h"

// ////////////////////////////////////////////////////////////////////////
// ToString
//
// Return: information about the node as a string

std::string Node::ToString() const
{
  return "[" + std::to_string(x) + "|" + std::to_string(y) + "]";
}

// ////////////////////////////////////////////////////////////////////////
// ToString
//
// Return: information about the move as a string

std::string Move::ToString() const
{
  return "From " + From()->ToString() + " to " + To()->ToString();
}

// ////////////////////////////////////////////////////////////////////////
// From
//
// Return: the node where the piece started

std::shared_ptr<Node> Move::From() const
{
  return m_pOrigin;
}

// ////////////////////////////////////////////////////////////////////////
// To
//
// Return: the node where to piece is going to move to

std::shared_ptr<Node> Move::To() const
{
  return m_pOrigin->m_aNeighbours[m_nDirection];
}

// ////////////////////////////////////////////////////////////////////////
// ToString
//
// Return: information about the capture as a string

std::string Capture::ToString() const
{
  return "Capturing " + std::to_string(m_vecCaptureNodes.size()) + " nodes";
}

std::string Turn::ToString() const
{
  std::string ret = m_pMove->ToString() + " " + m_pCapture->ToString();

  if (m_pNextTurn != nullptr)
  {
    ret += " with " + std::to_string(GetTurnChainLength()) + " following turns";
  }

  return ret;
}

std::string Turn::ChainToString() const
{
  std::shared_ptr<const Turn> current_turn = shared_from_this();

  std::string ret = current_turn->m_pMove->From()->ToString();
  while (current_turn != nullptr)
  {
    ret += " -> " + current_turn->m_pMove->To()->ToString();
    current_turn = current_turn->m_pNextTurn;
  }

  return ret;
}
uint Turn::GetTurnChainLength() const
{
  std::shared_ptr<const Turn> current_turn = shared_from_this();
  uint length                              = 1;

  while (current_turn != nullptr)
  {
    current_turn = current_turn->m_pNextTurn;
    length++;
  }

  return length;
}

bool Turn::IsWithdraw() const
{
  // The turn is a withdraw if the first captured node is a neighbor of From.
  std::shared_ptr<Node> first_capture = m_pCapture->m_vecCaptureNodes.front();

  for (const auto& neighbour : m_pMove->From()->m_aNeighbours)
  {
    if (neighbour == first_capture)
    {
      return true;
    }
  }
  return false;
}

bool Turn::NodeAlreadyVisited(std::shared_ptr<Node> node) const
{
  if (node == nullptr)
  {
    return true;
  }

  bool visited                            = false;
  std::shared_ptr<const Turn> currentTurn = shared_from_this();

  while (currentTurn != nullptr)
  {
    if (currentTurn->m_pMove->From() == node)
    {
      visited = true;
    }
    currentTurn = currentTurn->m_pPreviousTurn;
  }

  return visited;
}

bool Turn::IsNewDirection(int direction) const
{
  return m_pMove->m_nDirection != direction;
}
