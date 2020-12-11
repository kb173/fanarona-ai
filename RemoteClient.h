#pragma once

#include "Client.h"
#include <memory>
#include <string>

// ########################################################################
// Enumerations & Constants
// ########################################################################

const int READ_DATA_SIZE = 2048;

class RemoteClient : public Client
{
private:
  // ########################################################################
  // Fields
  // ########################################################################

  int m_nSock;
  char m_cBuffer[READ_DATA_SIZE] = {0};

public:
  // ########################################################################
  // Methods
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // RemoteClient (Constructor)
  //

  RemoteClient(std::string strIp, int nPort);

  // ////////////////////////////////////////////////////////////////////////
  // ~RemoteClient (Destructor)
  //

  ~RemoteClient();

  // ////////////////////////////////////////////////////////////////////////
  // ReadString
  //
  // Reads a string from the server

  std::string ReadString();

  // ////////////////////////////////////////////////////////////////////////
  // WriteString
  //
  // Sends a string to the server

  void WriteString(std::string strInput);
};
