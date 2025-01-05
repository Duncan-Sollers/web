#include "common.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdlib.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")


int main(int argc, char* argv[]) {
  CommandLineOpts opts{argc, argv};  // initialize opts

  parseCommandLineOption(opts);

  WinNetSetup wns;

  #define DEFAULT_PORT "27015"
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

  SOCKET ClientSocket;

  ClientSocket = INVALID_SOCKET;

  // Accept a client socket
  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    std::cout<<"[Error] accept failed: " << WSAGetLastError() << std::endl;
    closesocket(ListenSocket);
    exit(EXIT_FAILURE);
  }

  #define DEFAULT_BUFLEN 512

  char recvbuf[DEFAULT_BUFLEN];
  int iSendResult;
  int recvbuflen = DEFAULT_BUFLEN;

  // Receive until the peer shuts down the connection
  do {
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
      printf("Bytes received: %d\n", iResult);

      // Echo the buffer back to the sender
      iSendResult = send(ClientSocket, recvbuf, iResult, 0);
      if (iSendResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
      }
      printf("Bytes sent: %d\n", iSendResult);
    } else if (iResult == 0)
      printf("Connection closing...\n");
    else {
      printf("recv failed: %d\n", WSAGetLastError());
      closesocket(ClientSocket);
      WSACleanup();
      return 1;
    }

  } while (iResult > 0);
  for (;;)
  message();
  exit(EXIT_SUCCESS);
}