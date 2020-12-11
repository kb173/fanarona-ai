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

// ////////////////////////////////////////////////////////////////////////
// GetTurnsPlayed
//
// Return: the amount of turns that have been played by the AI

int Client::GetTurnsPlayed()
{
  return m_pBoard->GetTurnsPlayed();
}

// ////////////////////////////////////////////////////////////////////////
// Start
//
// Starts the gameplay loop

void Client::Start()
{
  size_t sizePos = std::string::npos;
  while (true)
  {
    // Read incoming commands
    m_strRecv.append(ReadString());

    // Always parse in-game state if present and pass the game data to the Board
    if ((sizePos = m_strRecv.rfind(MSG_BOARD_HEADER)) != std::string::npos)
    {
      std::string strField = m_strRecv.substr(sizePos, BOARD_LENGTH);
      if (strField.length() >= BOARD_LENGTH)
      {
        // Remove newlines for combined parsing depending on different server versions
        strField.erase(remove(strField.begin(), strField.end(), ' '), strField.end());
        m_pBoard->Parse(strField);
      }
    }

    // Check which gameplay selection this string corresponds to (if any)
    // and set the `mode` accordingly
    for (const auto& pairMessageAndMove : m_mapMessageState)
    {
      if (m_strRecv.rfind(pairMessageAndMove.first) != std::string::npos)
      {
        std::string strInput = m_pBoard->GetPosition(pairMessageAndMove.second);
        WriteString(strInput);
        break; // m_strRecv is cleared after sending so no checking for other messages is needed
      }
    }

    // If that wasn't an in-game string, we're in the menu
    if (m_strRecv.length() > 0)
    {
      // Check which menu point this string corresponds to and send the result
      for (const auto& pairMessageAndWrite : m_mapMessageWrite)
      {
        if (m_strRecv.rfind(pairMessageAndWrite.first) != std::string::npos)
        {
          WriteString(pairMessageAndWrite.second);
          break;
        }
      }

      // end of game detected, one player won
      if ((sizePos = m_strRecv.rfind(GAME_OVER)) != std::string::npos)
      {
        std::cerr << "\r\nGAME OVER!\r\n"
                  << m_strRecv.substr(sizePos - 8, 13) << std::endl
                  << std::endl;
        return;
      }
    }
  }
}

// ////////////////////////////////////////////////////////////////////////
// SetBoard (Setter for p_Board)

void Client::SetBoard(std::shared_ptr<Board> pBoard)
{
  m_pBoard = pBoard;
}
