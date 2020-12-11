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

// ////////////////////////////////////////////////////////////////////////
// RemoteClient (Constructor)
//

RemoteClient::RemoteClient(std::string strIp, int nPort)
{
  m_message_write_map.insert(std::make_pair("Exit\r\n\r\nPlease choose your mode [0-2]", "0"));
  m_message_write_map.insert(std::make_pair("User\r\n\r\nPlease choose your mode [0-2]", "1"));
  m_message_write_map.insert(
    std::make_pair("Please choose wether you want the AI to start or not [0-1]", "1"));
  m_message_write_map.insert(std::make_pair("Do you want to continue with your turn [Y/N]?", "Y"));
  m_message_write_map.insert(std::make_pair("Do you want to surrender [Y/N]?", "N"));

  std::cout << "init listening socket on " << strIp << " nPort " << nPort << std::endl;

#ifdef _WIN32
  WSAData wsaData;
  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
  {
    fprintf(stderr, "WSAStartup failed.\n");
    exit(1);
  }
#endif

  struct sockaddr_in serv_addr;
  if ((m_nSock = (int)socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    std::cout << "Socket creation error: " << m_nSock << std::endl;
    throw "Socket creation error.";
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = htons(nPort); // NOLINT

  // Convert IPv4 and IPv6 addresses from text to binary form
  int nStatus = 0;
  if ((nStatus = inet_pton(AF_INET, strIp.c_str(), &serv_addr.sin_addr)) <= 0)
  {
    std::cout << "Invalid address/ Address not supported > nStatus: " << nStatus << std::endl;
    throw "Invalid address/ Address not supported";
  }

  if ((nStatus = connect(m_nSock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
  {
    std::cout << "Connection failed! > nStatus: " << nStatus << " errno: " << errno << std::endl;
    throw "Connection Failed";
  }

  std::cout << "Connected!" << std::endl;
}

// ////////////////////////////////////////////////////////////////////////
// ~RemoteClient (Destructor)
//

RemoteClient::~RemoteClient()
{
#ifdef _WIN32
  closesocket(m_nSock);
#else
  close(m_nSock);
#endif
}

// ////////////////////////////////////////////////////////////////////////
// ReadString
//
// Reads a string from the server

std::string RemoteClient::ReadString()
{
  memset(m_cBuffer, 0, READ_DATA_SIZE);
#ifdef _WIN32
  if (recv(m_nSock, m_cBuffer, READ_DATA_SIZE, 0) < 0)
#else
  if (read(m_nSock, m_cBuffer, READ_DATA_SIZE) < 0)
#endif
  {
    throw "Receive Failed";
  }
#ifdef SHOW_RAW_MSG
  std::cout << "<<< received RAW string:\r\n" << m_cBuffer << "\r\n ### end RAW ###\r\n";
#endif
  return std::string(m_cBuffer);
}

// ////////////////////////////////////////////////////////////////////////
// WriteString
//
// Sends a string to the server

void RemoteClient::WriteString(std::string strInput)
{
  // Append newline if not present
  if (strInput.rfind("\n") != strInput.length())
  {
    strInput.append("\n");
  }

#ifdef SHOW_RAW_MSG
  std::cout << ">>> sending string: " << strInput << "### end send ###\r\n";
#endif
  if (send(m_nSock, strInput.c_str(), (int)strInput.length(), 0) < 0)
  {
    throw "Send Failed";
  }
  m_strRecv.clear();
}