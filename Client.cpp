#define DEBUG_OUTPUT

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

#include "Client.h"

Client::Client(std::string ip, int port)
{

#ifdef DEBUG_OUTPUT
    std::cout << "init listening socket on " << ip << " port " << port << std::endl;
#endif

#ifdef _WIN32
    //WSADATA wsaData; // if this doesn't work
    WSAData wsaData; // then try this instead

    // MAKEWORD(1,1) for Winsock 1.1, MAKEWORD(2,0) for Winsock 2.0
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
    serv_addr.sin_port = port;
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

    if ((status = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
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
#ifdef _WIN32
    recv(sock, buffer, READ_DATA_SIZE, 0);
#else
    read(sock, buffer, READ_DATA_SIZE);
#endif
    return std::string(buffer);
}

void Client::writeString(std::string input)
{
    send(sock, input.c_str(), input.length(), 0);
}
