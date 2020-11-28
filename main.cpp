#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string.h>

#include "Board.h"
#include "RemoteClient.h"
#include "LocalClient.h"

int main(int argc, char** argv)
{
  EMode mode = EMode::AI;
  int depth  = 3;
  // default values for server connection
  // "178.32.219.65", 4455 // Kajetan Test server - alpaga.hammerle.me
  // "10.64.99.107", 4455 // Lars' Fanorona Server
  // "127.0.0.1", 8888 // local Test server
  std::string ip = "178.32.219.65";
  int port       = 4455;
  bool local = true;
  for (int i = 0; i < argc; ++i)
  {
    if (strcmp(argv[i], "--human") == 0)
    {
      mode = EMode::HUMAN;
    }
    else if (strcmp(argv[i], "--depth") == 0)
    {
      depth = std::stoi(argv[i + 1]);
    }
    else if (strcmp(argv[i], "--ip") == 0)
    {
      ip = argv[i + 1];
    }
    else if (strcmp(argv[i], "--remote") == 0)
    {
     local = false;
    }
    else if (strcmp(argv[i], "--port") == 0)
    {
     port = std::stoi(argv[i + 1]);
    }
  }

  // our game board -> gets filled by server messages, calculates next position...
  auto board = std::make_shared<Board>(mode, depth);

  auto start = std::chrono::high_resolution_clock::now();
  if(local) {
    LocalClient client = LocalClient();
    client.SetBoard(board);
    client.Start();
  } else {
    RemoteClient client(ip, port);
    client.SetBoard(board);
    client.Start();
  }
  std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
  std::cerr << "time spent: " << std::setprecision(3) << std::fixed << diff.count() << "s"
            << std::endl;

  return 0;
}
