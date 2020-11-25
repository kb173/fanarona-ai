#pragma once

#include <memory>

// Forward Declarations
class Board;

const int READ_DATA_SIZE = 2048;

class Client
{
public:
  Client(std::string ip, int port);
  ~Client();

  std::string ReadString();
  void WriteString(std::string input);

  void SetBoard(std::shared_ptr<Board>);
  void Start();

private:
  int m_sock;
  char m_buffer[READ_DATA_SIZE] = {0};
  std::string m_strRecv         = "";

  std::shared_ptr<Board> m_board;
};
