#pragma once

#include <string>

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

  void SetBoard(Board*);
  void Start();

private:
  int m_sock;
  char m_buffer[READ_DATA_SIZE] = {0};
  std::string m_strRecv         = "";

  Board* m_board;
};
