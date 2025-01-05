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
  struct addrinfo *result = NULL, *ptr = nullptr, hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;


  // Resolve the server address and port
  const char* server = opts.server == nullptr ? DEFAULT_SERVER : opts.server;
  const char* port = opts.port == nullptr ? DEFAULT_PORT : opts.port;
  std::cout << "Server: " << server << "\nPort: " << port << std::endl;
  auto iResult = getaddrinfo(server, port, &hints, &result);
  if (iResult != 0) {
    std::cout << "[Error] getaddrinfo failed: " << iResult << std::endl;
    exit(EXIT_FAILURE);
  }

  SOCKET ConnectSocket = INVALID_SOCKET;

  // Attempt to connect to the first address returned by
  // the call to getaddrinfo
  ptr = result;

  // Create a SOCKET for connecting to server
  ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

  if (ConnectSocket == INVALID_SOCKET) {
    std::cout << "[Error] at socket(): " << WSAGetLastError() << std::endl;
    freeaddrinfo(result);
    exit(EXIT_FAILURE);
  }

  // Connect to server.
  iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    closesocket(ConnectSocket);
    ConnectSocket = INVALID_SOCKET;
    std::cout << "[Error] at connect():" << WSAGetLastError() << std::endl;
    exit(EXIT_FAILURE);
  }

  const char* httpRequest =
      "GET / HTTP / 1.0\r\nHost : localHost";

  #define DEFAULT_BUFLEN 512

  int recvbuflen = DEFAULT_BUFLEN;

  const char* sendbuf = httpRequest;
  char recvbuf[DEFAULT_BUFLEN];


  // Send an initial buffer
  iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
  if (iResult == SOCKET_ERROR) {
    printf("send failed: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
  }

  printf("Bytes Sent: %ld\n", iResult);

  // shutdown the connection for sending since no more data will be sent
  // the client can still use the ConnectSocket for receiving data
  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
  }

  // Receive data until the server closes the connection
  do {
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
      printf("Bytes received: %d\n", iResult);
    else if (iResult == 0)
      printf("Connection closed\n");
    else
      printf("recv failed: %d\n", WSAGetLastError());
  } while (iResult > 0);
  // Should really try the next address returned by getaddrinfo
  // if the connect call failed
  // But for this simple example we just free the resources
  // returned by getaddrinfo and print an error message

  freeaddrinfo(result);
  result = nullptr;

  



  message(); 

  exit(EXIT_SUCCESS);
}