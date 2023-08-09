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

  // check for invalid number of arguments provided
  if (argc < 5)
  {
    fprintf(stderr, "4 arguments required\n");
    exit(1);
  }
  
  // parse and assign arguements
  char *address = argv[1];
  short int port = atoi(argv[2]);
  char *username = argv[3];
  char *filename = argv[4];

  struct sockaddr_in addy;

  memset(&addy, 0, sizeof(struct sockaddr_in));
  addy.sin_family = AF_INET;
  addy.sin_port = htons(port);
  if (inet_pton(AF_INET, address, &addy.sin_addr) == 0) 
  {
    fprintf(stderr, "not an IPv4 address.\n");
    exit(2);
  }

  
  int cfd = socket(AF_INET, SOCK_STREAM, 0);

  if (cfd == -1) 
  {
    fprintf(stderr, "failed to create socket\n");
    exit(3);
  }

  if (connect(cfd, (struct sockaddr *)&addy, sizeof(struct sockaddr_in)) == -1)
  {
    fprintf(stderr, "failed to connect to server\n");
  }

  FILE *file = fopen(filename, "r+");

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char sizeStr[10];

  sprintf(sizeStr, "%d", size);


  write(cfd, username, strlen(username));
  write(cfd, filename, strlen(filename));
  write(cfd, sizeStr, strlen(sizeStr));

  char content[size];
  int i = 0;
  while (i < size) {
    if (fread((content + i), 1, 1, file) == -1)
      break;
    i++;
  }
  

  write(cfd, content, size);

  fclose(file);

  return 0;
}
