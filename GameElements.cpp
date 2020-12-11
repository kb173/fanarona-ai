#include "GameElements.h"

std::string Node::ToString() const
{
  return "[" + std::to_string(x) + "|" + std::to_string(y) + "]";
}

std::shared_ptr<Node> Move::From() const
{
  return m_origin;
}

std::shared_ptr<Node> Move::To() const
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

std::string Turn::ChainToString() const
{
  std::shared_ptr<const Turn> current_turn = shared_from_this();

  std::string ret = current_turn->move->From()->ToString();
  while (current_turn != nullptr)
  {
    ret += " -> " + current_turn->move->To()->ToString();
    current_turn = current_turn->nextTurn;
  }

  return ret;
}
uint Turn::GetTurnChainLength() const
{
  std::shared_ptr<const Turn> current_turn = shared_from_this();
  uint length                              = 1;

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
  std::shared_ptr<Node> first_capture = capture->capturedNodes.front();

  for (const auto& neighbour : move->From()->neighbours)
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

std::string Move::ToString() const
{
  return "From " + From()->ToString() + " to " + To()->ToString();
}
