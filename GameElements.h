#pragma once

#include <array>
#include <string>
#include <vector>

#ifdef _WIN32
typedef unsigned int uint;
#endif

enum class EState : char
{
  EMPTY,
  WHITE,
  BLACK,
  CURRENT,
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
  EState state                    = EState::EMPTY;
  std::array<Node*, 8> neighbours = {};

  Node() = default;
  Node(int x, int y) : x(x), y(y)
  {
  }

  std::string ToString() const;
};

struct Move
{
private:
  Node* m_origin;

public:
  Move(Node* origin, int direction) : m_origin(origin), direction(direction)
  {
  }

  std::string ToString() const;

  Node* From() const;
  Node* To() const;

  int direction;
};

struct Capture
{
  Capture(std::vector<Node*> capturedNodes) : capturedNodes(capturedNodes)
  {
  }

  std::string ToString() const;

  std::vector<Node*> capturedNodes;
};

struct Turn
{
  Turn(Move* move, Capture* capture) :
    move(move), capture(capture), nextTurn(nullptr), previousTurn(nullptr)
  {
  }

  std::string ToString() const;

  uint GetTurnChainLength() const;

  bool IsWithdraw() const;

  bool NodeAlreadyVisited(Node*) const;

  bool IsNewDirection(int) const;

  Move* move;
  Capture* capture;
  Turn* nextTurn;
  Turn* previousTurn;
};