#ifdef _DEBUG
  #define SHOW_RAW_MSG
#endif

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
#include "RemoteClient.h"

RemoteClient::RemoteClient(std::string ip, int port)
{
  message_write_map.insert(std::make_pair("Exit\r\n\r\nPlease choose your mode [0-2]", "0"));
  message_write_map.insert(std::make_pair("User\r\n\r\nPlease choose your mode [0-2]", "1"));
  message_write_map.insert(std::make_pair("Please choose wether you want the AI to start or not [0-1]", "1"));
  message_write_map.insert(std::make_pair("Do you want to continue with your turn [Y/N]?", "Y"));
  message_write_map.insert(std::make_pair("Do you want to surrender [Y/N]?", "Y"));

  std::cout << "init listening socket on " << ip << " port " << port << std::endl;

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

RemoteClient::~RemoteClient()
{
#ifdef _WIN32
  closesocket(m_sock);
#else
  close(m_sock);
#endif
}

std::string RemoteClient::ReadString()
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

void RemoteClient::WriteString(std::string input)
{
  // Append newline if not present
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