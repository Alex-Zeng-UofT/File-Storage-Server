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
#include <sys/wait.h>

void ignore_sigpipe(void)
{
  struct sigaction myaction;

  myaction.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &myaction, NULL);
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

            char buffer;

            // username
            for (int i = 0; i < 10; i++) {
                if (read(cfd, &buffer, 1) == -1)
                    break;
                if (i == 9) {
                    write(cfd, "HDERR\n", 6);
                    exit(1);
                }
                if (buffer == '\n') {
                    username[i] = '\0';
                    break;
                }
                username[i] = buffer;
            }

            // filename
            for (int i = 0; i < 102; i++) {
                if (read(cfd, &buffer, 1) == -1)
                    break;
                if (i == 101) {
                    write(cfd, "HDERR\n", 6);
                    exit(1);
                }
                if (buffer == '\n') {
                    filename[i] = '\0';
                    break;
                }
                filename[i] = buffer;
            }

            // length of file
            for (int i = 0; i < 12; i++) {
                if (read(cfd, &buffer, 1) == -1)
                    break;
                if (i == 11) {
                    write(cfd, "HDERR\n", 6);
                    exit(1);
                }
                if (buffer == '\n') {
                    length[i] = '\0';
                    break;
                }
                length[i] = buffer;
            }

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
            read(cfd, content, size);

            fwrite(content, 1, size, file);

            fclose(file);

            char message[12];
            sprintf(message, "%d", serial);
            strcat(message, "\n");

            write(cfd, message, strlen(message));
            
            return 0;
        }
    }
  }

  while (wait(NULL) > 0) {

  }

  close(sfd);

  return 0;
}
