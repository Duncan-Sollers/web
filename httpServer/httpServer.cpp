#include "common.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")
void processRequest(SOCKET ClientSocket);
std::string readfile();

int main(int argc, char* argv[]) {
  CommandLineOpts opts{argc, argv};  // initialize opts

  parseCommandLineOption(opts);

  WinNetSetup wns;
  //set up server socket
  struct addrinfo *result = nullptr, *ptr = nullptr, hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the local address and port to be used by the server
  const char* server = opts.server == nullptr ? DEFAULT_SERVER : opts.server;
  const char* port = opts.port == nullptr ? DEFAULT_PORT : opts.port;

  std::cout << "Server: " << server << "\nPort: " << port << std::endl;

  auto iResult = getaddrinfo(server, port, &hints, &result);
  if (iResult != 0) {
    std::cout<<"[Error] getaddrinfo failed: " << iResult << std::endl;
    exit(EXIT_FAILURE);
  }

  SOCKET ListenSocket = INVALID_SOCKET;

  // Create a SOCKET for the server to listen for client connections

  ListenSocket =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  if (ListenSocket == INVALID_SOCKET) {
    std::cout<<"[Error] at socket(): " << WSAGetLastError() << std::endl;
    freeaddrinfo(result);
    result = nullptr;
    exit(EXIT_FAILURE);
  }

  // Setup the TCP listening socket
  iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    std::cout<<"[Error] bind failed with error: " << WSAGetLastError()<<std::endl;
    freeaddrinfo(result);
    result = nullptr;
    closesocket(ListenSocket);
    exit(EXIT_FAILURE);
  }

  if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
    std::cout<<"[Error] Listen failed with error: "<< WSAGetLastError() << std::endl;
    closesocket(ListenSocket);
    exit(EXIT_FAILURE);
  }
  for (;;) {
  

  SOCKET ClientSocket;

  ClientSocket = INVALID_SOCKET;

  // Accept a client socket
  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    std::cout<<"[Error] accept failed: " << WSAGetLastError() << std::endl;
    closesocket(ListenSocket);
    exit(EXIT_FAILURE);
  }
  // Lambda to invoke processRequest
  auto invokeProcessRequest = [](SOCKET socket) { processRequest(socket); };

  // Example usage of the lambda
    invokeProcessRequest(ClientSocket);
  }
  message();
}



void processRequest(SOCKET ClientSocket) {
#define DEFAULT_BUFLEN 2048

  char recvbuf[DEFAULT_BUFLEN];
  int iSendResult;
  int recvbuflen = DEFAULT_BUFLEN;

  // Receive until the peer shuts down the connection
  int iResult;
  //do {
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
      printf("Bytes received: %d\n", iResult);
    } else if (iResult == 0)
      printf("Connection closing...\n");
    else {
      printf("recv failed: %d\n", WSAGetLastError());
      closesocket(ClientSocket);
      exit(EXIT_FAILURE);
    }

  //} while (iResult > 0);
  // Echo the buffer back to the sender
    std::string str = readfile();
    //std::string str = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: 19\r\n\r\n<title>Hello,World!\r\n";
    int size = str.size();
    iSendResult = send(ClientSocket, str.c_str(), str.size(), 0);

  if (iSendResult == SOCKET_ERROR) {
    printf("send failed: %d\n", WSAGetLastError());
    closesocket(ClientSocket);
    exit(EXIT_FAILURE);
  }
  printf("Bytes sent: %d\n", iSendResult);
  closesocket(ClientSocket);
}

std::string readfile() { 
  std::fstream file("test.txt");
  std::string line;
  std::string result;
  int i = 0;
  while (getline(file, line)) {
    i++;
    if (i == 3) {
      result += line + "\r\n" + "\r\n";
    } else if(i<3){
      result += line + "\r\n";
    } else {
      result += line;
    }
    
  }

  return result;
}