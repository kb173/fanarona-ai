#pragma once

#include "Client.h"
#include <memory>
#include <string>

class LocalClient : public Client
{
public:
  // ########################################################################
  // Methods
  // ########################################################################

  // ////////////////////////////////////////////////////////////////////////
  // LocalClient (Constructor)
  //

  LocalClient();

  // ////////////////////////////////////////////////////////////////////////
  // ~LocalClient (Destructor)
  //

  ~LocalClient();

  // ////////////////////////////////////////////////////////////////////////
  // ReadString
  //
  // Reads a string over socat

  std::string ReadString();

  // ////////////////////////////////////////////////////////////////////////
  // WriteString
  //
  // Sends a string over socat

  void WriteString(std::string input);
};
