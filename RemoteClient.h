#pragma once

#include "Client.h"
#include <memory>
#include <string>

// Forward Declarations
const int READ_DATA_SIZE = 2048;

class RemoteClient : public Client
{
public:
  RemoteClient(std::string ip, int port);
  ~RemoteClient();
  std::string ReadString();
  void WriteString(std::string input);

private:
  int m_sock;
  char m_buffer[READ_DATA_SIZE] = {0};
};
