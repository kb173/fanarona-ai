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
    int sock;
    char buffer[READ_DATA_SIZE] = {0};
    std::string strRecv = "";

    Board* board;
};
