#include "GameElements.h"

std::string Node::ToString() const
{
  return std::to_string(x) + " " + std::to_string(y);
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
  return "Capturing " + std::to_string(capturedNodes.size()) + " nodes";
}

std::string Turn::ToString() const
{
  std::string ret = move->ToString() + " " + capture->ToString();

  if (nextTurn != nullptr)
  {
    ret += " with " + std::to_string(GetTurnChainLength()) + " following turns";
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

bool Turn::NodeAlreadyVisited(Node* node) const
{
  if (node == nullptr)
  {
    return true;
  }

  bool visited            = false;
  const Turn* currentTurn = this;

  while (currentTurn != nullptr)
  {
    if (currentTurn->move->From() == node)
    {
      visited = true;
    }
    currentTurn = currentTurn->previousTurn;
  }

  return visited;
}

bool Turn::IsNewDirection(int direction) const
{
  return move->direction != direction;
}