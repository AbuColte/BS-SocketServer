#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "database.h"


typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} KeyValuePair;

KeyValuePair store[MAX_PAIRS];
int storeSize = 0;

// Set key-value
int put(const char *key, const char *value) {
    // Check if key already exists; if so, update
    for (int i = 0; i < storeSize; ++i) {
        if (strcmp(store[i].key, key) == 0) {
            strncpy(store[i].value, value, MAX_VALUE_LEN);
            return 2;
        }
    }

    // Add new key-value pair
    if (storeSize < MAX_PAIRS) {
        strncpy(store[storeSize].key, key, MAX_KEY_LEN);
        strncpy(store[storeSize].value, value, MAX_VALUE_LEN);
        storeSize++;
    } else {
        printf("Store full!\n");
    }
}

// Get value by key

int get(const char *key) {
    char tempString[1024];
    printf("GET Function accessed \n");
    for (int i = 0; i < strlen(key) - 1; ++i) {
        printf("%c\n", key[i]);
        tempString[i] = key[i];
    }
    for (int i = 0; i < storeSize; i++) {
        if (strcmp(store[i].key, tempString) == 0) {
            printf("Found!\n");
            printf("Value : %s\n", store[i].value);
            printf("Key : %s\n", store[i].key);
            printf("StoreIndex : %d\n", i);
            return 1;
        }
    }
    printf("Not Found-\n");
    return 0;
}

int del(char *key) {
    char tempString[1024];
    printf("DEL Function accessed \n");
    for (int i = 0; i < strlen(key) - 1; ++i) {
        printf("%c\n", key[i]);
        tempString[i] = key[i];
    }
    for (int i = 0; i < storeSize; i++) {
        if (strcmp(store[i].key, tempString) == 0) {
            for (int j = i; j < storeSize - 1; j++) {
                store[j] = store[j + 1];
            }
            storeSize--;  // Reduce count
            printf("Deleted key: %s\n", key);
            return 1;
        }
    }
    printf("Not Found-\n");
    return 0;
}

int main() {
    // Server Socket
    fd_set CurrentSockets, ReadySockets;
    FD_ZERO(&CurrentSockets);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("ERROR opening socket\n");
    }
    FD_SET(sockfd,  &CurrentSockets);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(1234);

    // char buffer[1024];
    int addressLength = sizeof(address);

    bind(sockfd, (struct sockaddr*)&address, addressLength) ? printf("ERROR on bind\n") : printf("OK on bind\n");

    listen(sockfd, 5) ? printf("ERROR on listen\n") : printf("OK on listen\n");

    // int client_socket = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addressLength);
    // if (client_socket < 0) {
    //     printf("ERROR on accept\n");
    // }
    // write(client_socket, "Hello World!\n", 14);
    // char string[1024] = {0};  // Buffer to store input
    // int i = 0;                // Current position in the buffer

    while (1) {
    ReadySockets = CurrentSockets;
    if (select(FD_SETSIZE, &ReadySockets, NULL, NULL, NULL) < 0) {
        perror("select");
        exit(1);
    }

    for (int i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, &ReadySockets)) {
            if (i == sockfd) {
                int client_socket = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addressLength);
                if (client_socket < 0) {
                    perror("accept");
                    continue;
                }
                FD_SET(client_socket, &CurrentSockets);
            } else {
                char string[1024] = {0};
                char buffer[1024];
                int bufIndex = 0;

                int bytesRead = read(i, buffer, sizeof(buffer) - 1);
                if (bytesRead <= 0) {
                    // Client disconnected or error
                    if (bytesRead == 0) {
                        printf("Client disconnected\n");
                    } else {
                        perror("read");
                    }
                    close(i);
                    FD_CLR(i, &CurrentSockets);
                    continue;
                }

                buffer[bytesRead] = '\0';

                for (int u = 0; u < bytesRead; u++) {
                    if (buffer[u] == '\n') {
                        string[bufIndex] = '\0';
                        printf("Full input: %s\n", string);

                        if (strncmp(string, "quit", 4) == 0) {
                            printf("Received quit command\n");
                            close(i);
                            FD_CLR(i, &CurrentSockets);
                            break;
                        }

                        if (strncmp(string, "put", 3) == 0) {
                            char *command = strtok(string, " ");
                            char *key = strtok(NULL, " ");
                            char *value = strtok(NULL, " ");
                            if (key && value) {
                                put(key, value);
                                write(i, "OK\n", 3);
                            }
                        } else if (strncmp(string, "get", 3) == 0) {
                            char *command = strtok(string, " ");
                            char *key = strtok(NULL, " ");
                            if (key && get(key)) {
                                write(i, "FOUND\n", 6);
                            } else {
                                write(i, "NOT FOUND\n", 10);
                            }
                        } else if (strncmp(string, "del", 3) == 0) {
                            char *command = strtok(string, " ");
                            char *key = strtok(NULL, " ");
                            if (key && del(key)) {
                                write(i, "DELETED\n", 8);
                            } else {
                                write(i, "NOT FOUND\n", 10);
                            }
                        }

                        bufIndex = 0;
                        memset(string, 0, sizeof(string));
                    } else {
                        if (bufIndex < sizeof(string) - 1) {
                            string[bufIndex++] = buffer[u];
                        }
                    }
                }
            }
        }
    }
}

    exit_loop:

    close(sockfd);

    return 0;
}