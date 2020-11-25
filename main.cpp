#include <chrono>
#include <iomanip>
#include <iostream>

#include <string.h>

#include "Board.h"
#include "Client.h"

int main(int argc, char** argv)
{
  EMode mode = EMode::AI;
  // default values for server connection
  // "178.32.219.65", 4455 // Kajetan Test server - alpaga.hammerle.me
  // "10.64.99.107", 4455 // Lars' Fanorona Server
  // "127.0.0.1", 8888 // local Test server
  std::string ip = "178.32.219.65";
  int port       = 4455;
  for (int i = 0; i < argc; ++i)
  {
    if (strcmp(argv[i], "--human") == 0)
    {
      mode = EMode::HUMAN;
    }
    else if (strcmp(argv[i], "--ip") == 0)
    {
      ip = argv[i + 1];
    }
    else if (strcmp(argv[i], "--port") == 0)
    {
      port = std::stoi(argv[i + 1]);
    }
  }

  // our game board -> gets filled by server messages, calculates next position...
  auto board = std::make_shared<Board>(mode);

  auto start = std::chrono::high_resolution_clock::now();
  Client client(ip, port);
  client.SetBoard(board);
  client.Start();
  std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
  std::cout << "time spent: " << std::setprecision(3) << std::fixed << diff.count() << "s"
            << std::endl;

  return 0;
}
