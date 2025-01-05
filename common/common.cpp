#include "common.h"

#include <iostream>
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "ya_getopt.h"



const char* DEFAULT_PORT = "31415";
const char* DEFAULT_SERVER = "localhost";

void message() { std::cout << " Hello World! " << std::endl; }

void parseCommandLineOption(CommandLineOpts& opts) {
  int c;
  while ((c = ya_getopt(opts.argc, opts.argv, "p:s:")) != -1) {
    switch (c) {
      case 'p':
        opts.port = ya_optarg;
        break;
      case 's':
        opts.server = ya_optarg;
        break;
      default:
        break;
    }
  }
}