#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string.h>

#include "Board.h"
#include "LocalClient.h"
#include "RemoteClient.h"

// ////////////////////////////////////////////////////////////////////////
// ShowStartUpImage
//
// Prints an ASCII image to the error output on start up
void ShowStartUpImage()
{
  std::string strLine;
  std::ifstream ssImage("startupImage.txt");
  if (ssImage.is_open())
  {
    while (getline(ssImage, strLine))
    {
      std::cerr << strLine << std::endl;
    }
    ssImage.close();
  }
  else
  {
    std::cerr << "Unable to open startup image" << std::endl;
  }
}
// ////////////////////////////////////////////////////////////////////////
// main
//
//
int main(int argc, char** argv)
{
  ShowStartUpImage();
  EMode eMode = EMode::AI;
  int nDepth  = 3;
  // default values for server connection
  // "178.32.219.65", 4455 // Kajetan Test server - alpaga.hammerle.me
  // "10.64.99.107", 4455 // Lars' Fanorona Server
  // "127.0.0.1", 8888 // local Test server
  std::string strIp = "178.32.219.65";
  int nPort         = 4455;
  bool bLocal       = true;
  for (int i = 0; i < argc; ++i)
  {
    if (strcmp(argv[i], "--human") == 0)
    {
      eMode = EMode::HUMAN;
    }
    else if (strcmp(argv[i], "--depth") == 0)
    {
      nDepth = std::stoi(argv[i + 1]);
    }
    else if (strcmp(argv[i], "--ip") == 0)
    {
      strIp = argv[i + 1];
    }
    else if (strcmp(argv[i], "--remote") == 0)
    {
      bLocal = false;
    }
    else if (strcmp(argv[i], "--port") == 0)
    {
      nPort = std::stoi(argv[i + 1]);
    }
  }

  // our game board -> gets filled by server messages, calculates next position...
  auto board = std::make_shared<Board>(eMode, nDepth);

  auto clockStart = std::chrono::high_resolution_clock::now();
  if (bLocal)
  {
    LocalClient client = LocalClient();
    client.SetBoard(board);
    client.Start();
    std::cerr << "turns played: " << client.GetTurnsPlayed() << std::endl;
  }
  else
  {
    RemoteClient client(strIp, nPort);
    client.SetBoard(board);
    client.Start();
    std::cerr << "turns played: " << client.GetTurnsPlayed() << std::endl;
  }
  std::chrono::duration<double> clockDiff = std::chrono::high_resolution_clock::now() - clockStart;
  std::cerr << "time spent: " << std::setprecision(3) << std::fixed << clockDiff.count() << "s"
            << std::endl;

  return 0;
}
