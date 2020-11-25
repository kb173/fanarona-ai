#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

typedef unsigned int uint;

enum class EState : char
{
  EMPTY,
  WHITE,
  BLACK
};

enum class EMove : char
{
  ORIGIN_X,
  ORIGIN_Y,
  DEST_X,
  DEST_Y,
  W_OR_A
};

struct Node
{
  int x, y;
  EState state                                    = EState::EMPTY;
  std::array<std::shared_ptr<Node>, 8> neighbours = {};

  Node() = default;
  Node(int x, int y) : x(x), y(y)
  {
  }

  std::string ToString() const;
};

struct Move
{
private:
  std::shared_ptr<Node> m_origin;

public:
  Move(std::shared_ptr<Node> origin, int direction) : m_origin(origin), direction(direction)
  {
  }

  std::string ToString() const;

  std::shared_ptr<Node> From() const;
  std::shared_ptr<Node> To() const;

  int direction;
};

struct Capture
{
  Capture(std::vector<std::shared_ptr<Node>> capturedNodes) : capturedNodes(capturedNodes)
  {
  }

  std::string ToString() const;

  std::vector<std::shared_ptr<Node>> capturedNodes;
};

struct Turn : public std::enable_shared_from_this<Turn>
{
  Turn(std::shared_ptr<Move> move, std::shared_ptr<Capture> capture) :
    move(move), capture(capture), nextTurn(nullptr), previousTurn(nullptr)
  {
  }

  std::string ToString() const;

  uint GetTurnChainLength() const;

  bool IsWithdraw() const;

  bool NodeAlreadyVisited(std::shared_ptr<Node>) const;

  bool IsNewDirection(int) const;

  std::shared_ptr<Move> move;
  std::shared_ptr<Capture> capture;
  std::shared_ptr<Turn> nextTurn;
  std::shared_ptr<Turn> previousTurn;
};
