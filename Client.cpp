#define DEBUG_OUTPUT
#define SHOW_RAW_MSG

#ifdef _WIN32
  // link with Ws2_32.lib
  #pragma comment(lib, "Ws2_32.lib")

  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <arpa/inet.h>
  #include <sys/socket.h>
  #include <unistd.h>
#endif
#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <string>

#include "Board.h"
#include "Client.h"

#define BOARD_LENGTH 380 // SERVER_V2 total gridsize including whitespaces

#define MSG_BOARD_HEADER "   0   1   2   3   4   5   6   7   8"
#define MSG_PLAYMODE                                                                               \
  "Please choose your mode [0-2]" // select Start, rules, exit and AI or HUMAN player
#define MSG_PLAYERSTART                                                                            \
  "Please choose wether you want the AI to start or not [0-1]"       // select starting player
#define MSG_CONTINUE "Do you want to continue with your turn [Y/N]?" // make another turn?

std::map<std::string, EMove> message_state_map {
  {"Please enter origin x-axis", EMove::ORIGIN_X},
  {"Please enter origin y-axis", EMove::ORIGIN_Y},
  {"Please enter destination x-axis", EMove::DEST_X},
  {"Please enter destination y-axis", EMove::DEST_Y},
  {"Please enter wether you want to Withdraw or Approach [W/A]", EMove::W_OR_A}};

std::map<std::string, std::string> message_write_map {
  {"Exit\r\n\r\nPlease choose your mode [0-2]", "0"},
  {"User\r\n\r\nPlease choose your mode [0-2]", "1"},
  {"Please choose wether you want the AI to start or not [0-1]", "1"},
  {"Do you want to continue with your turn [Y/N]?", "Y"},
  {"Do you want to surrender [Y/N]?", "Y"}};

Client::Client(std::string ip, int port)
{
#ifdef DEBUG_OUTPUT
  std::cout << "init listening socket on " << ip << " port " << port << std::endl;
#endif

#ifdef _WIN32
  WSAData wsaData;
  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
  {
    fprintf(stderr, "WSAStartup failed.\n");
    exit(1);
  }
#endif

  struct sockaddr_in serv_addr;
  if ((m_sock = (int)socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    std::cout << "Socket creation error: " << m_sock << std::endl;
    throw "Socket creation error.";
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = htons(port);
  // serv_addr.sin_addr.s_addr = inet_addr(ip.c_str()); // deprecated -> use inet_pton instead

  // Convert IPv4 and IPv6 addresses from text to binary form
  int status = 0;
  if ((status = inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)) <= 0)
  {
    std::cout << "Invalid address/ Address not supported > status: " << status << std::endl;
    throw "Invalid address/ Address not supported";
  }

  if ((status = connect(m_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
  {
    std::cout << "Connection failed! > status: " << status << " errno: " << errno << std::endl;
    throw "Connection Failed";
  }

  std::cout << "Connected!" << std::endl;
}

Client::~Client()
{
#ifdef _WIN32
  closesocket(m_sock);
#else
  close(m_sock);
#endif
}

std::string Client::ReadString()
{
  memset(m_buffer, 0, READ_DATA_SIZE);
#ifdef _WIN32
  if (recv(m_sock, m_buffer, READ_DATA_SIZE, 0) < 0)
#else
  if (read(m_sock, m_buffer, READ_DATA_SIZE) < 0)
#endif
  {
    throw "Receive Failed";
  }
#ifdef SHOW_RAW_MSG
  std::cout << "<<< received RAW string:\r\n" << m_buffer << "\r\n ### end RAW ###\r\n";
#endif
  return std::string(m_buffer);
}

void Client::WriteString(std::string input)
{
  // append newline if not present
  if (input.rfind("\n") != input.length())
  {
    input.append("\n");
  }

#ifdef SHOW_RAW_MSG
  std::cout << ">>> sending string: " << input << "### end send ###\r\n";
#endif
  if (send(m_sock, input.c_str(), (int)input.length(), 0) < 0)
  {
    throw "Send Failed";
  }
  m_strRecv.clear();
}

void Client::SetBoard(Board* i_board)
{
  m_board = i_board;
}

void Client::Start()
{
  size_t pos = std::string::npos;
  while (true)
  {
    m_strRecv.append(ReadString());

    EMove mode;

    bool in_game = false;

    // Check which gameplay selection this string corresponds to (if any)
    // and set the `mode` accordingly
    for (const auto& message_and_move : message_state_map)
    {
      if (m_strRecv.rfind(message_and_move.first) != std::string::npos)
      {
        mode    = message_and_move.second;
        in_game = true;

        // Since we found an in-game state, pass the game data to the Board
        if ((pos = m_strRecv.rfind(MSG_BOARD_HEADER)) != std::string::npos)
        {
          std::string field = m_strRecv.substr(pos, BOARD_LENGTH);

          // remove newlines for combined parsing depending on different server versions
          field.erase(remove(field.begin(), field.end(), ' '), field.end());
          m_board->Parse(field);
        }

        // usleep(300000);

        std::string input = m_board->GetPosition(mode);
        WriteString(input);
      }
    }

    // If that wasn't an in-game string, we're in the menu
    if (!in_game)
    {
      // Check which menu point this string corresponds to and send the result
      for (const auto& message_and_write : message_write_map)
      {
        if (m_strRecv.rfind(message_and_write.first) != std::string::npos)
        {
          WriteString(message_and_write.second);
        }
      }
    }
  }
}
