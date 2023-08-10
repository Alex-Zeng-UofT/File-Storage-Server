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

  FILE *file = fopen(argv[4], "r+");

  // prepare for connection
  struct sockaddr_in addy;

  memset(&addy, 0, sizeof(struct sockaddr_in));
  addy.sin_family = AF_INET;
  addy.sin_port = htons(port);
  if (inet_pton(AF_INET, address, &addy.sin_addr) == 0) 
  {
    fprintf(stderr, "not an IPv4 address.\n");
    exit(2);
  }
 
  // create client socket
  int cfd = socket(AF_INET, SOCK_STREAM, 0);

  if (cfd == -1) 
  {
    fprintf(stderr, "failed to create socket\n");
    exit(3);
  }

  // attempt to connect
  if (connect(cfd, (struct sockaddr *)&addy, sizeof(struct sockaddr_in)) == -1)
  {
    fprintf(stderr, "failed to connect to server\n");
  }
  
  // get size of the file
  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  char sizeStr[20];

  sprintf(sizeStr, "%d", size);
  strcat(sizeStr, "\n");

  // write username, filename, and file size
  write(cfd, argv[3], strlen(argv[3]));
  write(cfd, "\n", 1);
  write(cfd, argv[4], strlen(argv[4]));
  write(cfd, "\n", 1);
  write(cfd, sizeStr, strlen(sizeStr));

  // buffer for individual content
  char content;

  // content into server 1 byte at a time
  for (int i = 0; i < size; i++) {
    if (fread(&content, 1, 1, file) == -1 || write(cfd, &content, 1) == -1)
      break;
  }

  fclose(file);

  return 0;
}
