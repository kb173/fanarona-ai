//#define DEBUG_OUTPUT
//#define SHOW_RAW_MSG

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
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include "Board.h"
#include "Client.h"

#define SERVER_V1 1
#define SERVER_V2 2

#define SERVER_VERSION SERVER_V2
#if SERVER_VERSION == SERVER_V1
#define MSG_BOARD_HEADER "  0 1 2 3 4 5 6 7 8"          // start of server message for current board state
#define MSG_STONE "select stone:"                       // select stone
#define MSG_SELECT_LOCATION "select location to move:"  // location for current stone
#define MSG_CAPTURE "select stone to take: "            // stone to capture for multiple choices
#elif  SERVER_VERSION == SERVER_V2
#define MSG_BOARD_HEADER "   0   1   2   3   4   5   6   7   8"
#define MSG_STONE "Please enter origin"                 // x - axis, y-axis
#define MSG_SELECT_LOCATION "Please enter destination"  // x - axis, y-axis
#define MSG_CAPTURE "Please enter wether you want to Withdraw or Approach [W/A]"
#define MSG_PLAYMODE "Please choose your mode [0-2]"    // select Start, rules, exit and AI or HUMAN player
#define MSG_PLAYERSTART "Please choose wether you want the AI to start or not [0-1]"    // select starting player
#define MSG_CONTINUE "Do you want to continue with your turn [Y/N]?" // make another turn?
#endif

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
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout << "Socket creation error: " << sock << std::endl;
        throw "Socket creation error.";
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // serv_addr.sin_addr.s_addr = inet_addr(ip.c_str()); // deprecated -> use inet_pton instead

    // Convert IPv4 and IPv6 addresses from text to binary form
    int status = 0;
    if ((status = inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)) <= 0)
    {
        std::cout << "Invalid address/ Address not supported > status: " << status << std::endl;
        throw "Invalid address/ Address not supported";
    }

    if ((status = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        std::cout << "Connection failed! > status: " << status << " errno: " << errno << std::endl;
        throw "Connection Failed";
    }

    std::cout << "Connected!" << std::endl;
}

Client::~Client()
{
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

std::string Client::ReadString()
{
    memset(buffer, 0, READ_DATA_SIZE);
#ifdef _WIN32
    if (recv(sock, buffer, READ_DATA_SIZE, 0) < 0)
#else
    if (read(sock, buffer, READ_DATA_SIZE) < 0)
#endif
    {
        throw "Receive Failed";
    }
#ifdef SHOW_RAW_MSG
    std::cout << "<<< received RAW string:\r\n" << buffer << "\r\n###\r\n";
#endif
    return std::string(buffer);
}

void Client::WriteString(std::string input)
{
    // append newline if not present
    if (input.rfind("\n") != input.length())
    {
        input.append("\n");
    }

#ifdef SHOW_RAW_MSG
    std::cout << ">>> sending string:\r\n" << input << "\r\n###\r\n";
#endif
    if (send(sock, input.c_str(), (int)input.length(), 0) < 0)
    {
        throw "Send Failed";
    }
    strRecv.clear();
}

void Client::SetBoard(Board* i_board)
{
    board = i_board;
}

void Client::Start()
{
    size_t pos = std::string::npos;
    while (true)
    {
        strRecv.append(ReadString());

        EMove mode = EMove::NONE;
        if (strRecv.find(MSG_CONTINUE) != std::string::npos)
        {
            WriteString("Y"); // never give up, never surrender!
        }
        else if (strRecv.find(MSG_CAPTURE) != std::string::npos)
        {
            mode = EMove::CAPTURE;
        }
        else if (strRecv.find(MSG_SELECT_LOCATION) != std::string::npos)
        {
            mode = EMove::LOCATION;
        }
        else if (strRecv.find(MSG_STONE) != std::string::npos)
        {
            mode = EMove::STONE;
        }
        else if (strRecv.find(MSG_PLAYMODE) != std::string::npos)
        {
            static int type = 0;
            if (type == 0)
            {
                WriteString("0");
                type++;
            }
            else
            {
                WriteString("1");
            }
        }
        else if (strRecv.find(MSG_PLAYERSTART) != std::string::npos)
        {
            WriteString("1"); // we go always second... if not -> TODO: configure as param/property
        }
        else
        {
#ifdef DEBUG_OUTPUT
            std::cout << "/// currently unhandled text buffer:\n'" << strRecv << "'" << std::endl;
#endif
        }

        if (mode != EMove::NONE)
        {
            if ((pos = strRecv.rfind(MSG_BOARD_HEADER)) != std::string::npos)
            {
                // TODO: make const for fixed size length
                // std::string field = strRecv.substr(pos, 201);
                // std::string field = strRecv.substr(pos, 209);
                std::string field = strRecv.substr(pos, 380);

                // remove newlines for combined parsing depending on different server versions
                field.erase(remove(field.begin(), field.end(), ' '), field.end());
                board->Parse(field);
            }

            std::string input = board->GetPosition(mode);
            WriteString(input);
        }
    }
}
