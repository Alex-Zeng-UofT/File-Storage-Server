#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024];

    char username[9];
    char filename[101];
    char length[11];

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    // if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == 0) 
    // {
    //     fprintf(stderr, "not an IPv4 address.\n");
    //     exit(2);
    // }
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(44015);  // Port to listen on

    // Bind the socket to the address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 44015...\n");

    int serial = 0;

    while (1) {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        } else {
            serial++;
        }

        char buffer;

        for (int i = 0; i < 10; i++) {
            if (read(client_socket, &buffer, 1) == -1)
                break;
            if (i == 9) {
                write(client_socket, "HDERR\n", 6);
                exit(1);
            }
            if (buffer == '\n') {
                username[i] = '\0';
                break;
            }
            username[i] = buffer;
        }

        for (int i = 0; i < 102; i++) {
            if (read(client_socket, &buffer, 1) == -1)
                break;
            if (i == 101) {
                write(client_socket, "HDERR\n", 6);
                exit(1);
            }
            if (buffer == '\n') {
                filename[i] = '\0';
                break;
            }
            filename[i] = buffer;
        }

        for (int i = 0; i < 12; i++) {
            if (read(client_socket, &buffer, 1) == -1)
                break;
            if (i == 11) {
                write(client_socket, "HDERR\n", 6);
                exit(1);
            }
            if (buffer == '\n') {
                length[i] = '\0';
                break;
            }
            length[i] = buffer;
        }

        int size = atoi(length);

        printf("%s\n", username);
        printf("%s\n", filename);
        printf("%d\n", serial);
        printf("%d\n", size);

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

        printf("%s\n", name);

        FILE *file = fopen(name, "w");

        char content[size];
        read(client_socket, content, size);

        fwrite(content, 1, size, file);

        fclose(file);

        printf("Client disconnected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        close(client_socket);

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
