#pragma once

#include <list>

#include "AIPlayer.h"
#include "GameElements.h"

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
  EMode m_mode;
  std::shared_ptr<Node> m_cells[BOARD_HEIGHT][BOARD_WIDTH] = {};
  AIPlayer m_player;

public:
  Board(EMode mode, int depth);
  ~Board() = default;

  void Parse(std::string boardContent);
  void Print();

  // Return the String which the Client should send to the Server next.
  std::string GetPosition(EMove);

  // Return a list of all possible turns which the given Node could take.
  const std::list<std::shared_ptr<Turn>> FindTurnsForNode(EState,
                                                          std::shared_ptr<Node>,
                                                          std::shared_ptr<Turn>,
                                                          bool includePaika);

  const std::list<std::shared_ptr<Turn>> GenerateTurnsWithFollowingTurns(
    std::shared_ptr<Turn> startTurn,
    EState pieceColor);

  // Return a list of all possible turns which any Node on the field with the given state could
  // take.
  const std::list<std::shared_ptr<Turn>> FindTurns(EState);

  // Apply a turn to the Board by moving the corresponding Node and removing Captures.
  void ApplyTurn(std::shared_ptr<Turn>);

  // Rollback a turn which was previously applied with ApplyTurn.
  void RollbackTurn(std::shared_ptr<Turn>);

  void ApplyTurnWithFollowing(std::shared_ptr<Turn>);

  void RollbackTurnWithFollowing(std::shared_ptr<Turn>);

  std::shared_ptr<Node> GetCell(int x, int y);

  inline bool IsPositionInBounds(int x, int y);

  // returns the distance to the neares enemy piece of the piece on the passed node, return -1 if
  // the node is empty or no enemy was found,  needed for rating function
  int DistanceToNearestEnemy(std::shared_ptr<Node>);
  int DistanceBetweenNodes(std::shared_ptr<Node>, std::shared_ptr<Node>);
  // returns all pieces captured by a move, either in the direction of the move or behind the
  // move.
  const std::vector<std::shared_ptr<Node>> GetCapturesInDirection(const Move& move,
                                                                  bool reverse_direction);

  int GetTurnsPlayed();
};
