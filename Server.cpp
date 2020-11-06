/*
    C socket server example from https://www.binarytides.com/server-client-example-c-sockets-linux/
*/

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
#include <string>
#ifdef DEBUG_OUTPUT
#include <iostream>
#endif

int Server(std::string ip, int port)
{

#ifdef _WIN32
    // WSADATA wsaData; // if this doesn't work
    WSAData wsaData; // then try this instead

    // MAKEWORD(1,1) for Winsock 1.1, MAKEWORD(2,0) for Winsock 2.0
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }
#endif

    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server, client;
    char client_message[2000];

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind
    if (bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        // print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    // Listen
    listen(socket_desc, 3);

    // Accept and incoming connection
    std::cout << "Waiting for incoming connections on " << ip << ":" << port << "..." << std::endl;
    c = sizeof(struct sockaddr_in);

    // accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    // Receive a message from client
    while ((read_size = recv(client_sock, client_message, 2000, 0)) > 0)
    {
        // Send the message back to client
        // write(client_sock, client_message, strlen(client_message));
        send(client_sock, client_message, std::string(client_message).length(), 0);
    }

    if (read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if (read_size == -1)
    {
        perror("recv failed");
    }
    return 0;
}

int main()
{

    Server("127.0.0.1", 8888);
}