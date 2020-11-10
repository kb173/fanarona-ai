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

    std::string readString();
    void writeString(std::string input);

    void setBoard(Board*);
    void start();

  private:
    int sock;
    char buffer[READ_DATA_SIZE] = {0};

    Board* board;
};
