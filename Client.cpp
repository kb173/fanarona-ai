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
#include <stdio.h>
#include <string.h>
#include <string>
#ifdef DEBUG_OUTPUT
#include <iostream>
#endif

#include "Board.h"
#include "Client.h"

#define MSG_BOARD_HEADER "  0 1 2 3 4 5 6 7 8"         // start of server message for current board state
#define MSG_SELECT_STONE "select stone:"               // message to select stone
#define MSG_SELECT_LOCATION "select location to move:" // message to select location for current stone
#define MSG_SELECT_CAPTURE "select stone to take: "    // message to select stone to capture for multiple choices

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
#ifdef DEBUG_OUTPUT
        std::cout << "Socket creation error: " << sock << std::endl;
#endif
        throw "Socket creation error.";
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // serv_addr.sin_addr.s_addr = inet_addr(ip.c_str()); // deprecated -> use inet_pton instead

    // Convert IPv4 and IPv6 addresses from text to binary form
    int status = 0;
    if ((status = inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)) <= 0)
    {
#ifdef DEBUG_OUTPUT
        std::cout << "Invalid address/ Address not supported > status: " << status << std::endl;
#endif
        throw "Invalid address/ Address not supported";
    }

    if ((status = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
#ifdef DEBUG_OUTPUT
        std::cout << "Connection failed! > status: " << status << " errno: " << errno << std::endl;
#endif
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

std::string Client::readString()
{
    memset(buffer, 0, READ_DATA_SIZE);
#ifdef _WIN32
    if (recv(sock, buffer, READ_DATA_SIZE, 0) < 0)
#else
    if (read(sock, buffer, READ_DATA_SIZE) < 0)
#endif
        throw "Receive Failed";
    return std::string(buffer);
}

void Client::writeString(std::string input)
{
    if (send(sock, input.c_str(), (int)input.length(), 0) < 0)
        throw "Send Failed";
}

void Client::setBoard(Board* i_board)
{
    board = i_board;
}

void Client::start()
{
    size_t pos = std::string::npos;
    std::string recv = "";
    while (true)
    {
        recv.append(readString());
#ifdef SHOW_RAW_MSG
        std::cout << "### RAW\r\n" << recv << "\r\n###\r\n";
#endif

        pos = std::string::npos;
        Mode mode = Mode::SELECT_INVALID;
        if (recv.find(MSG_SELECT_STONE) != std::string::npos)
        {
            mode = Mode::SELECT_STONE;
        }
        else if (recv.find(MSG_SELECT_LOCATION) != std::string::npos)
        {
            mode = Mode::SELECT_MOVEMENT;
        }
        else if (recv.find(MSG_SELECT_CAPTURE) != std::string::npos)
        {
            mode = Mode::SELECT_CAPTURE;
        }
        else
        {
#ifdef DEBUG_OUTPUT
            std::cout << "/// unhandled text:\n'" << recv << "'" << std::endl;
#endif
        }

        if (mode != Mode::SELECT_INVALID)
        {
            if ((pos = recv.rfind(MSG_BOARD_HEADER)) != std::string::npos)
            {
                // std::string field = recv.substr(pos, 201); // TODO: make const for fixed size length
                std::string field = recv.substr(pos, 209); // TODO: make const for fixed size length
                board->parse(field);
            }

            std::string input = board->getPosition(mode);
            writeString(input + "\n");

            recv.clear();
        }
    }
}