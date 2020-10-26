#include <string>

class Client {
public:
  Client(std::string ip, int port);
  ~Client();

  std::string readString();
  void writeString(std::string input);

private:
  int sock;
  char buffer[2048] = {0}; 
};
