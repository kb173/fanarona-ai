#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>

#include "Board.h"
#include "LocalClient.h"

LocalClient::LocalClient()
{
  m_message_write_map.insert(std::make_pair("Exit\r\nPlease choose your mode [0-2]", "0"));
  m_message_write_map.insert(std::make_pair("User\r\nPlease choose your mode [0-2]", "1"));
  m_message_write_map.insert(
    std::make_pair("Please choose wether you want the AI to start or not [0-1]", "1"));
  m_message_write_map.insert(std::make_pair("Do you want to continue with your turn [Y/N]?", "Y"));
  m_message_write_map.insert(std::make_pair("Do you want to surrender [Y/N]?", "Y"));
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
  m_strRecv.clear();
}
