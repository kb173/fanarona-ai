
#pragma once

#include <memory>
#include <string>
#include <map>

class Board;

class Client
{
public:
  virtual std::string ReadString() = 0;
  virtual void WriteString(std::string input) = 0;

  void SetBoard(std::shared_ptr<Board>);
  void Start();

protected:
  std::string m_strRecv         = "";

  std::map<std::string, EMove> message_state_map {
  {"Please enter origin x-axis", EMove::ORIGIN_X},
  {"Please enter origin y-axis", EMove::ORIGIN_Y},
  {"Please enter destination x-axis", EMove::DEST_X},
  {"Please enter destination y-axis", EMove::DEST_Y},
  {"Please enter wether you want to Withdraw or Approach [W/A]", EMove::W_OR_A}};

  std::map<std::string, std::string> message_write_map;

private:
  std::shared_ptr<Board> m_board;
};