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

void sendInfo(int cfd, char *content) {

  int length = strlen(content);

  for (int i = 0; i < length; i++) {
    if(write(cfd, (content + i), 1) == -1) {
      printf("error\n");
    }
  }
  if (write(cfd, "\n", 1) == -1) {
      printf("error\n");
  }
}

int check_valid(char *str) {

  int length = strlen(str);

  for (int i = 0; i < length; i++) {
    if (str[i] == '\n') {
      break;
    }
    if (str[i] < 48 && str[i] > 57) {
      return 0;
    }
  }

  return 1;
}

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

  FILE *file;
  if ((file = fopen(argv[4], "r+")) == NULL) {
    fprintf(stderr, "cannot open file\n");
    exit(10);
  }

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
  
  char sizeStr[30];
  sprintf(sizeStr, "%d", size);

  // write to server 1 byte at a time
  sendInfo(cfd, argv[3]);
  sendInfo(cfd, argv[4]);
  sendInfo(cfd, sizeStr);

  // buffer for individual content
  char content;

  // write content into server 1 byte at a time
  for (int i = 0; i < size; i++) {
    if (fread(&content, 1, 1, file) != 1 || write(cfd, &content, 1) == -1)
      break;
  }

  fclose(file);

  // load message from server
  char message[11];
  for (int i = 0; i < 11; i++) {
    read(cfd, message + i, 1);
  }
  
  // print output
  if (strncmp("HDERR", message, 5) == 0 || check_valid(message)) {
    printf("Message from server: %s\n", message);
  } else { // if message is not valid
    fprintf(stderr, "invalid format\n");
  }

  close(cfd);

  return 0;
}
