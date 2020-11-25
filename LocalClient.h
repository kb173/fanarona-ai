#pragma once

#include <memory>
#include <string>

// Forward Declarations
class Board;

class LocalClient
{
public:
  LocalClient();
  ~LocalClient();

  std::string ReadString();
  void WriteString(std::string input);

  void SetBoard(std::shared_ptr<Board>);
  void Start();

private:
  std::string m_strRecv = "";
  std::shared_ptr<Board> m_board;
};
