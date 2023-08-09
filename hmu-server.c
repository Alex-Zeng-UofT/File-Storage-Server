// Server program.

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


void ignore_sigpipe(void)
{
  struct sigaction myaction;

  myaction.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &myaction, NULL);
}


// cmdline reminder: portnum, helper
int main(int argc, char *argv[]) {
  // TODO
  if (argc < 3) {
    fprintf(stderr, "not enough arguments");
    exit(1);
  }

  ignore_sigpipe();

  while (1) {

  }

  return 0;
}
