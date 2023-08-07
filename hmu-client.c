// Client program.

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// cmdline reminder: IP-address, portnum, username, filename
int main(int argc, char *argv[]) {
  // TODO

  char *address = argv[1];
  int port = atoi(argv[2]);
  char *username = argv[3];
  char *filename = argv[4];

  printf("%s\n", address);
  printf("%d\n", port);
  printf("%s\n", username);
  printf("%s\n", filename);

  return 0;
}
