#include <iostream>
#include <string>

#include "Board.h"
#include "Client.h"

int main()
{
  // TODO: get Board Mode from arguments, also server to connect (or local?)
  Board* board = new Board (
    EMode::AI); // our game board -> gets filled by server messages, calculates next position...

  // Client client("10.64.99.107", 4455); // Lars' Fanorona Server
  // Client client("127.0.0.1", 8888); // Test server
  Client client ("178.32.219.65", 4455); // Kajetan Test server - alpaga.hammerle.me
  client.SetBoard (board);
  client.Start();

  return 0;
}
