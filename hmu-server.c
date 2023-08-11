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
#include<ctype.h>

// standard ignoring
void ignore_sigpipe(void)
{
  struct sigaction myaction;

  myaction.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &myaction, NULL);
}

// check for valid username
void checkU(char *storage, int len, int cfd) {

    char b;

    for (int i = 0; i < len + 2; i++) {
        if (read(cfd, &b, 1) == -1)
            break;
        if (i == len + 1 || isalnum(b) == 0 && b != '\n' || b == '\n' && i == 0) {
            write(cfd, "HDERR\n", 6);
            close(cfd);
            exit(1);
        }
        if (b == '\n') {
            storage[i] = '\0';
            break;
        }
        storage[i] = b;
    }
    return;
}

// check for valid filename
void checkF(char *storage, int len, int cfd) {

    char b;

    for (int i = 0; i < len + 2; i++) {
        if (read(cfd, &b, 1) == -1)
            break;
        if (i == len + 1 || b == 47 || b == '\n' && i == 0) {
            write(cfd, "HDERR\n", 6);
            close(cfd);
            exit(1);
        }
        if (b == '\n') {
            storage[i] = '\0';
            break;
        }
        storage[i] = b;
    }
    return;
}

// check for valid size number
void checkN(char *storage, int len, int cfd) {

    char b;

    for (int i = 0; i < len + 2; i++) {
        if (read(cfd, &b, 1) == -1)
            break;
        if (i == len + 1 || b != '\n' && b < 48 && b > 57 || b == '\n' && i == 0) {
            write(cfd, "HDERR\n", 6);
            close(cfd);
            exit(1);
        }
        if (b == '\n') {
            storage[i] = '\0';
            break;
        }
        storage[i] = b;
    }
    return;
}

// cmdline reminder: portnum, helper
int main(int argc, char *argv[]) {
  // TODO

  // check for invalid amount of arguments
  if (argc < 3) {
    fprintf(stderr, "not enough arguments");
    exit(1);
  }

  ignore_sigpipe();
  signal(SIGCHLD, SIG_IGN);

  int sfd;
  struct sockaddr_in sfd_a;
  int serial = 0;

  // create socket for server
  sfd = socket(AF_INET, SOCK_STREAM, 0);

  // set up for binding
  memset(&sfd_a, 0, sizeof(struct sockaddr_in));
  sfd_a.sin_family = AF_INET;
  sfd_a.sin_port = htons(atoi(argv[1]));
  sfd_a.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind socket
  if (bind(sfd, (struct sockaddr *)&sfd_a, sizeof(struct sockaddr_in)) == -1) {
    perror("bind");
    return 1;
  }

  // initiate listen for socket
  if (listen(sfd, 5) == -1) {
    perror("listen");
    return 1;
  }

  // infinite while loop
  while (1) {
    
    int cfd;
    struct sockaddr_in cfd_a;
    socklen_t cfd_len = sizeof(cfd_a);

    // variables to store client information
    char username[9];
    char filename[101];
    char length[11];

    // accept client
    cfd = accept(sfd, (struct sockaddr *)&cfd_a, &cfd_len);
    
    if (cfd == -1) {
        perror("Error accepting connection");
        continue;
    } else {
        serial++;

        int p = fork();

        if (p == 0) {

            close(sfd);

            char buffer;
            
            // username
            checkU(username, 8, cfd);

            // filename
            checkF(filename, 100, cfd);

            // length of file
            checkN(length, 10, cfd);

            int size = atoi(length);

            // convert 
            char s[20];
            sprintf(s, "%d", serial);

            // create new file name
            char name[150];
            strcpy(name, username);
            strcat(name, "-");
            strcat(name, s);
            strcat(name, "-");
            strcat(name, filename);

            FILE *file = fopen(name, "w");

            char content[size];
            int r_count = 0;
            int w_count = 0;

            // transfer content to server file
            for (int i = 0; i < size; i++) {
                if(read(cfd, content + i, 1) <= 0) break;
                r_count++;
                w_count += fwrite(content + i, 1, 1, file);
            }

            fclose(file);

            char message[12];
            sprintf(message, "%d", serial);
            strcat(message, "\n");

            // delete file if less that promised bytes
            if (r_count < size || w_count < size) {
                execlp("rm", "rm", name, (char*)NULL);
            } else { // send message one byte at a time
                    for (int i = 0; i < 12; i++) {
                    write(cfd, message + i, 1);
                    if (message[i] == '\n') {
                        message[i + 1] = '\0';
                        break;
                    }
                }
            }

            close(cfd);
            
            return 0;
        }
        close(cfd);
    }
  }

  close(sfd);

  return 0;
}
