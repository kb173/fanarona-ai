#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <string>

#include "Client.h"

Client::Client(std::string ip, int port)
{ 
    struct sockaddr_in serv_addr; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
      throw "Socket creation error.";
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = port;
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)  
    { 
        throw "Invalid address/ Address not supported";
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        throw "Connection Failed"; 
    }
}

Client::~Client() {
    close(sock);
}

std::string Client::readString() {
    read(sock, buffer, READ_DATA_SIZE);
    return std::string(buffer);
}

void Client::writeString(std::string input) {
    send(sock, input.c_str(), input.length(), 0); 
}
