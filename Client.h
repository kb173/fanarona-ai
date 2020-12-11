
#pragma once

#include "Board.h"
#include <map>
#include <memory>
#include <string>

class Client

{ // ########################################################################
  // Fields
  // ########################################################################
private:
  std::shared_ptr<Board> m_pBoard;

protected:
  std::string m_strRecv = "";

  std::map<std::string, EMove> m_mapMessageState {
    {"Please enter origin x-axis", EMove::ORIGIN_X},
    {"Please enter origin y-axis", EMove::ORIGIN_Y},
    {"Please enter destination x-axis", EMove::DEST_X},
    {"Please enter destination y-axis", EMove::DEST_Y},
    {"Please enter wether you want to Withdraw or Approach [W/A]", EMove::W_OR_A}};

  std::map<std::string, std::string> m_mapMessageWrite;

public:
  // ########################################################################
  // Methods
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // ReadString
  //
  // Reads in a string from an undefined source

  virtual std::string ReadString() = 0;

  // ////////////////////////////////////////////////////////////////////////
  // WriteString
  //
  // Writes a string to an undefined source

  virtual void WriteString(std::string strInput) = 0;

  // ////////////////////////////////////////////////////////////////////////
  // GetTurnsPlayed
  //
  // Return: the amount of turns that have been played by the AI
  int GetTurnsPlayed();

  // ////////////////////////////////////////////////////////////////////////
  // Start
  //
  // Starts the gameplay loop
  void Start();

  // ########################################################################
  // Getter & Setter
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // SetBoard (Setter for p_Board)
  void SetBoard(std::shared_ptr<Board>);
};