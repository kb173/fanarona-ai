#define DEBUG_OUTPUT
#define SHOW_RAW_MSG

#ifdef _WIN32
  // link with Ws2_32.lib
  #pragma comment(lib, "Ws2_32.lib")

  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <arpa/inet.h>
  #include <sys/socket.h>
  #include <unistd.h>
#endif
#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <string>

#include "Board.h"
#include "LocalClient.h"

#define BOARD_LENGTH 380 // SERVER_V2 total gridsize including whitespaces
#define MSG_BOARD_HEADER "0   1   2   3   4   5   6   7   8" // defines start of board
//"************************ Player 2 won!**********************"
#define GAME_OVER "won!**********************" // end of game

std::map<std::string, EMove> message_state_map_local {
  {"Please enter origin x-axis", EMove::ORIGIN_X},
  {"Please enter origin y-axis", EMove::ORIGIN_Y},
  {"Please enter destination x-axis", EMove::DEST_X},
  {"Please enter destination y-axis", EMove::DEST_Y},
  {"Please enter wether you want to Withdraw or Approach [W/A]", EMove::W_OR_A}};

std::map<std::string, std::string> message_write_map_local {
  {"Exit\r\nPlease choose your mode [0-2]", "0"},
  {"User\r\nPlease choose your mode [0-2]", "1"},
  {"Please choose wether you want the AI to start or not [0-1]", "1"},
  {"Do you want to continue with your turn [Y/N]?", "Y"},
  {"Do you want to surrender [Y/N]?", "Y"}};

 LocalClient::LocalClient()
 {

 }

 LocalClient::~LocalClient()
 {

 }

std::string LocalClient::ReadString()
{
  std::string s;
  std::getline(std::cin >> std::ws, s);
  return s + "\n";
}

void LocalClient::WriteString(std::string input)
{
  std::cout << input << std::endl;
}

void LocalClient::SetBoard(std::shared_ptr<Board> i_board)
{
  m_board = i_board;
}

void LocalClient::Start()
{
  size_t pos = std::string::npos;
  while (true)
  {
    m_strRecv.append(ReadString());
    EMove mode;

    bool in_game = false;

    // Check which gameplay selection this string corresponds to (if any)
    // and set the `mode` accordingly
    for (const auto& message_and_move : message_state_map_local)
    {
      if (m_strRecv.rfind(message_and_move.first) != std::string::npos)
      {
        mode    = message_and_move.second;
        in_game = true;

        // Since we found an in-game state, pass the game data to the Board
        if ((pos = m_strRecv.rfind(MSG_BOARD_HEADER)) != std::string::npos)
        {
          std::string field = m_strRecv.substr(pos, BOARD_LENGTH);
          // remove newlines for combined parsing depending on different server versions
          field.erase(remove(field.begin(), field.end(), ' '), field.end());
          m_board->Parse(field);
          
        }

        // usleep(300000);
        std::string input = m_board->GetPosition(mode);
        WriteString(input);
        m_strRecv = "";
      }
    }

    // If that wasn't an in-game string, we're in the menu
    if (!in_game)
    {
      // Check which menu point this string corresponds to and send the result
      for (const auto& message_and_write : message_write_map_local)
      {
        if (m_strRecv.rfind(message_and_write.first) != std::string::npos)
        {
          WriteString(message_and_write.second);
          m_strRecv = "";
        }
      }

      if (m_strRecv.rfind(GAME_OVER) != std::string::npos)
      {
          return;
      }
    }
  }
}
