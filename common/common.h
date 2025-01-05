#ifndef COMMON_H
#define COMMON_H

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

#define YA_GETOPT_NO_COMPAT_MACRO

#pragma comment(lib, "Ws2_32.lib")

void message();
//ports less than 1024 are reserved and usually require admin privledges
extern const char* DEFAULT_PORT;
extern const char* DEFAULT_SERVER;

class WinNetSetup {
 public:
  WinNetSetup() {
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
      std::cout << "[Err] WSAStartup failed: " << iResult << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  ~WinNetSetup() { WSACleanup(); }
};

struct CommandLineOpts {
  int argc;
  char** argv;
  char* port;
  char* server;
};

void parseCommandLineOption(CommandLineOpts& opts);

#endif