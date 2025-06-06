#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "database.h"
#define PORT 99889
#define MAX_CLIENTS  FD_SETSIZE
#define BUFFER_SIZE  1024

int main() {
    int listen_fd, new_fd, max_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;

    fd_set master_set, read_set;


    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);


    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }


    if (listen(listen_fd, 10) < 0) {
        perror("listen failed");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }


    FD_ZERO(&master_set);
    FD_SET(listen_fd, &master_set);
    max_fd = listen_fd;

    printf("Server listening on port %d...\n", PORT);


    while (1) {
        read_set = master_set;

        if (select(max_fd + 1, &read_set, NULL, NULL, NULL) < 0) {
            perror("select failed");
            exit(EXIT_FAILURE);
        }


        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &read_set)) {
                if (fd == listen_fd) {

                    addr_len = sizeof(client_addr);
                    new_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &addr_len);
                    if (new_fd < 0) {
                        perror("accept failed");
                        continue;
                    }
                    FD_SET(new_fd, &master_set);
                    if (new_fd > max_fd) max_fd = new_fd;

                    printf("New connection from %s:%d (fd: %d)\n",
                        inet_ntoa(client_addr.sin_addr),
                        ntohs(client_addr.sin_port),
                        new_fd);

                } else {
                    char string[1024] = {0};
                    int j = 0;
                    char buffer[1024];
                    while(1) {
                        int bytesRead = read(fd, buffer, 1024);
        if (bytesRead < 0) {
            printf("ERROR on read\n");
            close(fd);
            FD_CLR(fd, &master_set);
            break;
        }
        buffer[bytesRead] = '\0';


        for (int u = 0; u < bytesRead; u++) {
            if (buffer[u] == '\n') {
                string[j] = '\0';
                printf("Full input: %s\n", string);

                // Check if input is "quit"
                if (
              string[0] == 'q' &&
              string[1] == 'u' &&
              string[2] == 'i' &&
              string[3] == 't') {
                    string[j] = '\0';
                    printf("Received quit command\n");
                    FD_CLR(fd, &master_set);
                    break;
              }

                // Check if input starts with "put"
                if (string[0] == 'p' && string[1] == 'u' && string[2] == 't') {
                    char *command = strtok(string, " ");

                    if (command && strcmp(command, "put") == 0) {
                        char *key = strtok(NULL, " ");
                        char *value = strtok(NULL, " ");

                        if (key && value) {
                            printf("Key: %s\n", key);
                            printf("Value: %s\n", value);
                            put(key, value);
                        } else {
                            printf("Error: key or value missing.\n");
                        }
                    } else {
                        printf("Not a 'put' command.\n");
                    }
                    memset(string, 0, sizeof(string));
                }

                if (string[0] == 'g' && string[1] == 'e' && string[2] == 't') {
                    char *command = strtok(string, " ");

                    if (command && strcmp(command, "get") == 0) {
                        char *key = strtok(NULL, " ");
                        printf(" %d\n", get(key));
                        if (key) {
                            printf("At Key: %s\n", key);
                        } else {
                            printf("Error: key or value missing.\n");
                        }
                    } else {
                        printf("Not a 'put' command.\n");
                    }
                    memset(string, 0, sizeof(string));
                }

                if(string[0] == 'd' && string[1] == 'e' && string[2] == 'l') {
                    char *command = strtok(string, " ");

                    if (command && strcmp(command, "del") == 0) {
                        char *key = strtok(NULL, " ");
                        printf(" %d\n", del(key));
                        if (key) {
                            printf("Key deleted: %s\n", key);
                        } else {
                            printf("Error: key or value missing.\n");
                        }
                    } else {
                        printf("Not a 'del' command.\n");
                    }
                    memset(string, 0, sizeof(string));
                }

                j = 0;  // Reset buffer for next input
            } else {
                string[j++] = buffer[u];  // Store the character
                write(fd, buffer, strlen(buffer) - 1);
                // Prevent buffer overflow
                if (j >= sizeof(string) - 1) {
                    j = 0;  // Reset or handle overflow
                }
            }
        }
        //memset(string, 0, sizeof(string));

        memset(buffer, 0, 1024);  // Clear the read buffer
                    }
                }
            }
        }
        }
    close(listen_fd);
    return 0;
}