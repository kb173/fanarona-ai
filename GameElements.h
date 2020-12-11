#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

typedef unsigned int uint;

// ########################################################################
// Enumerations & Constants
// ########################################################################

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

// ########################################################################
// Structs
// ########################################################################

struct Node
{
  // ########################################################################
  // Fields
  // ########################################################################

  int x, y;
  EState eState                                      = EState::EMPTY;
  std::array<std::shared_ptr<Node>, 8> m_aNeighbours = {};
  bool m_bIsMiddleField                              = false;
  bool m_bIsDiagonalField                            = false;

  // ########################################################################
  // Methods
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // Node (Constructor)
  //

  Node() = default;

  // ////////////////////////////////////////////////////////////////////////
  // Node (Constructor)
  //

  Node(int x, int y) : x(x), y(y)
  {
  }

  // ////////////////////////////////////////////////////////////////////////
  // ToString
  //
  // Return: information about the node as a string

  std::string ToString() const;
};

struct Move
{
  // ########################################################################
  // Fields
  // ########################################################################
  std::shared_ptr<Node> m_pOrigin;

  int m_nDirection;

  // ########################################################################
  // Methods
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // Move (Constructor)
  //

  Move(std::shared_ptr<Node> pOrigin, int nDirection) : m_pOrigin(pOrigin), m_nDirection(nDirection)
  {
  }

  // ////////////////////////////////////////////////////////////////////////
  // ToString
  //
  // Return: information about the move as a string

  std::string ToString() const;

  // ////////////////////////////////////////////////////////////////////////
  // From
  //
  // Return: the node where the piece started

  std::shared_ptr<Node> From() const;

  // ////////////////////////////////////////////////////////////////////////
  // To
  //
  // Return: the node where to piece is going to move to

  std::shared_ptr<Node> To() const;
};

struct Capture
{
  // ########################################################################
  // Fields
  // ########################################################################

  std::vector<std::shared_ptr<Node>> m_vecCaptureNodes;

public:
  // ########################################################################
  // Methods
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // Capture (Constructor)
  //

  Capture(std::vector<std::shared_ptr<Node>> vecCaptureNodes) : m_vecCaptureNodes(vecCaptureNodes)
  {
  }

  // ////////////////////////////////////////////////////////////////////////
  // ToString
  //
  // Return: information about the capture as a string

  std::string ToString() const;
};

struct Turn : public std::enable_shared_from_this<Turn>
{
  // ########################################################################
  // Fields
  // ########################################################################

  std::shared_ptr<Move> m_pMove;
  std::shared_ptr<Capture> m_pCapture;
  std::shared_ptr<Turn> m_pNextTurn;
  std::shared_ptr<Turn> m_pPreviousTurn;

  // ########################################################################
  // Methods
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // Turn (Constructor)
  //

  Turn(std::shared_ptr<Move> pMove, std::shared_ptr<Capture> pCapture) :
    m_pMove(pMove), m_pCapture(pCapture), m_pNextTurn(nullptr), m_pPreviousTurn(nullptr)
  {
  }

  // ////////////////////////////////////////////////////////////////////////
  // ToString
  //
  // Return: information about the turn as a string

  std::string ToString() const;

  // ////////////////////////////////////////////////////////////////////////
  // ChainToString (Constructor)
  //
  // Return: information about the turn chain as a string

  std::string ChainToString() const;

  // ////////////////////////////////////////////////////////////////////////
  // GetTurnChainLength
  //

  uint GetTurnChainLength() const;

  // ////////////////////////////////////////////////////////////////////////
  // IsWithdraw
  //

  bool IsWithdraw() const;

  // ////////////////////////////////////////////////////////////////////////
  // NodeAlreadyVisited
  //

  bool NodeAlreadyVisited(std::shared_ptr<Node>) const;

  // ////////////////////////////////////////////////////////////////////////
  // IsNewDirection
  //

  bool IsNewDirection(int) const;
};
