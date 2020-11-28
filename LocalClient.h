#pragma once

#include <memory>
#include <string>
#include "Client.h"

// Forward Declarations
class LocalClient : public Client
{
public:
  LocalClient();
  ~LocalClient();
  std::string ReadString();
  void WriteString(std::string input);
};
