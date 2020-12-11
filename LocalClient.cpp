#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>

#include "Board.h"
#include "LocalClient.h"

// ////////////////////////////////////////////////////////////////////////
// LocalClient (Constructor)
//

LocalClient::LocalClient()
{
  m_message_write_map.insert(std::make_pair("Exit\r\nPlease choose your mode [0-2]", "0"));
  m_message_write_map.insert(std::make_pair("User\r\nPlease choose your mode [0-2]", "1"));
  m_message_write_map.insert(
    std::make_pair("Please choose wether you want the AI to start or not [0-1]", "1"));
  m_message_write_map.insert(std::make_pair("Do you want to continue with your turn [Y/N]?", "Y"));
  m_message_write_map.insert(std::make_pair("Do you want to surrender [Y/N]?", "N"));
}

// ////////////////////////////////////////////////////////////////////////
// ~LocalClient (Destructor)
//

LocalClient::~LocalClient()
{
}

// ////////////////////////////////////////////////////////////////////////
// ReadString
//
// Reads a string over socat

std::string LocalClient::ReadString()
{
  std::string str;
  std::getline(std::cin >> std::ws, str);
  return str + "\n";
}

// ////////////////////////////////////////////////////////////////////////
// WriteString
//
// Sends a string over socat

void LocalClient::WriteString(std::string strInput)
{
  std::cout << strInput << std::endl;
  m_strRecv.clear();
}
