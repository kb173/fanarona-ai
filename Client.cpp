#include "Client.h"
#include "Board.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <stdio.h>

#define BOARD_LENGTH     380 // SERVER_V2 total gridsize including whitespaces
#define MSG_BOARD_HEADER "0   1   2   3   4   5   6   7   8" // defines start of board
//"************************ Player 2 won!**********************"
#define GAME_OVER " won!**********************" // end of game

void Client::SetBoard(std::shared_ptr<Board> i_board)
{
  m_board = i_board;
}
int Client::GetTurnsPlayed()
{
  return m_board->GetTurnsPlayed();
}
void Client::Start()
{
  size_t pos = std::string::npos;
  while (true)
  {
    // Read incoming commands
    m_strRecv.append(ReadString());

    // Always parse in-game state if present and pass the game data to the Board
    if ((pos = m_strRecv.rfind(MSG_BOARD_HEADER)) != std::string::npos)
    {
      std::string field = m_strRecv.substr(pos, BOARD_LENGTH);
      if (field.length() >= BOARD_LENGTH)
      {
        // Remove newlines for combined parsing depending on different server versions
        field.erase(remove(field.begin(), field.end(), ' '), field.end());
        m_board->Parse(field);
      }
    }

    // Check which gameplay selection this string corresponds to (if any)
    // and set the `mode` accordingly
    for (const auto& message_and_move : message_state_map)
    {
      if (m_strRecv.rfind(message_and_move.first) != std::string::npos)
      {
        std::string input = m_board->GetPosition(message_and_move.second);
        WriteString(input);
        break; // m_strRecv is cleared after sending so no checking for other messages is needed
      }
    }

    // If that wasn't an in-game string, we're in the menu
    if (m_strRecv.length() > 0)
    {
      // Check which menu point this string corresponds to and send the result
      for (const auto& message_and_write : m_message_write_map)
      {
        if (m_strRecv.rfind(message_and_write.first) != std::string::npos)
        {
          WriteString(message_and_write.second);
          break;
        }
      }

      // end of game detected, one player won
      if ((pos = m_strRecv.rfind(GAME_OVER)) != std::string::npos)
      {
        std::cerr << "\r\nGAME OVER!\r\n"
                  << m_strRecv.substr(pos - 8, 13) << std::endl
                  << std::endl;
        return;
      }
    }
  }
}
