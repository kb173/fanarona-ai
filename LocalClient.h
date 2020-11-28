#pragma once

#include "Client.h"
#include <memory>
#include <string>

// Forward Declarations
class LocalClient : public Client
{
public:
  LocalClient();
  ~LocalClient();
  std::string ReadString();
  void WriteString(std::string input);
};
