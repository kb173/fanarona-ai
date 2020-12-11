#pragma once

#include <list>

#include "AIPlayer.h"
#include "GameElements.h"

// ########################################################################
// Enumerations & Constants
// ########################################################################

//#define OMIT_OUTPUT

const int BOARD_WIDTH  = 9;
const int BOARD_HEIGHT = 5;

enum class EMode : char
{
  HUMAN,
  AI,
};

class Board
{
private:
  // ########################################################################
  // Fields
  // ########################################################################

  EMode m_eMode;
  std::shared_ptr<Node> m_pCells[BOARD_HEIGHT][BOARD_WIDTH] = {};
  AIPlayer m_aiPlayer;

public:
  // ########################################################################
  // Methods
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // Board (Constructor)
  //

  Board(EMode eMode, int nDepth);

  // ////////////////////////////////////////////////////////////////////////
  // ~Board (Destructor)
  //

  ~Board() = default;

  // ////////////////////////////////////////////////////////////////////////
  // Parse
  //
  // Parameter: string of board content received from the server
  // Method parses this string and creates board content from it

  void Parse(std::string strBoardContent);

  // ////////////////////////////////////////////////////////////////////////
  // Print
  //
  // Prints the board

  void Print();

  // ////////////////////////////////////////////////////////////////////////
  // GetPosition
  //
  // Return:  the String which the Client should send to the Server next

  std::string GetPosition(EMove);

  // ////////////////////////////////////////////////////////////////////////
  // FindTurnsForNode
  //
  // Return:  a list of all possible turns which the given Node could take

  const std::list<std::shared_ptr<Turn>> FindTurnsForNode(EState,
                                                          std::shared_ptr<Node>,
                                                          std::shared_ptr<Turn>,
                                                          bool bIncludePaika);

  // ////////////////////////////////////////////////////////////////////////
  // GenerateTurnsWithFollowingTurns
  //
  // Generates all combination of valid follow up turns for a given turn

  const std::list<std::shared_ptr<Turn>> GenerateTurnsWithFollowingTurns(
    std::shared_ptr<Turn> pStartTurn,
    EState ePieceColor);

  // ////////////////////////////////////////////////////////////////////////
  // FindTurns
  //
  // Return:  a list of all possible turns which any Node on the field with
  //          the given state could take.
  const std::list<std::shared_ptr<Turn>> FindTurns(EState);

  // ////////////////////////////////////////////////////////////////////////
  // ApplyTurn
  //
  // Apply a turn to the Board by moving the corresponding Node and removing Captures.

  void ApplyTurn(std::shared_ptr<Turn>);

  // ////////////////////////////////////////////////////////////////////////
  // RollbackTurn
  //
  // Rollback a turn which was previously applied with ApplyTurn.

  void RollbackTurn(std::shared_ptr<Turn>);

  // ////////////////////////////////////////////////////////////////////////
  // ApplyTurnWithFollowing
  //
  // Apply a turn to the Board including its follow up turns.

  void ApplyTurnWithFollowing(std::shared_ptr<Turn>);

  // ////////////////////////////////////////////////////////////////////////
  // RollbackTurnWithFollowing
  //
  // Rollback a turn including its follow up turns which was previously applied with ApplyTurn.

  void RollbackTurnWithFollowing(std::shared_ptr<Turn>);

  // ////////////////////////////////////////////////////////////////////////
  // GetCell
  //

  std::shared_ptr<Node> GetCell(int x, int y);

  // ////////////////////////////////////////////////////////////////////////
  // IsPositionInBounds
  //

  inline bool IsPositionInBounds(int x, int y);

  // ////////////////////////////////////////////////////////////////////////
  // DistanceToNearestEnemy
  //
  // Return: the distance to the nearest enemy, neede for some heuristics

  int DistanceToNearestEnemy(std::shared_ptr<Node>);

  // ////////////////////////////////////////////////////////////////////////
  // DistanceBetweenNodes
  //
  // Return: the distance between two nodes using valid movement on the board

  int DistanceBetweenNodes(std::shared_ptr<Node>, std::shared_ptr<Node>);

  // ////////////////////////////////////////////////////////////////////////
  // GetCapturesInDirection
  //
  // return: all pieces captured by a move, either in the direction of the move or behind the
  // move.

  const std::vector<std::shared_ptr<Node>> GetCapturesInDirection(const Move& move,
                                                                  bool bReverse_direction);

  // ////////////////////////////////////////////////////////////////////////
  // GetTurnsPlayed
  //
  // Return: how many turns the AIPlayer has played

  int GetTurnsPlayed();
};
