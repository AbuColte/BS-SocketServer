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

void trim_newline(char *str) {
    str[strcspn(str, "\r\n")] = 0;
}


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

int del(const char *key) {
    char trimmedKey[MAX_KEY_LEN];
    strncpy(trimmedKey, key, MAX_KEY_LEN - 1);
    trimmedKey[MAX_KEY_LEN - 1] = '\0';
    trim_newline(trimmedKey);

    for (int i = 0; i < storeSize; i++) {
        if (strcmp(store[i].key, trimmedKey) == 0) {
            for (int j = i; j < storeSize - 1; j++) {
                store[j] = store[j + 1];
            }
            storeSize--;
            return 1;  // Deleted
        }
    }
    return 0;  // Not found
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
        if(select(FD_SETSIZE, &ReadySockets, NULL, NULL, NULL) < 0) {
            printf("ERROR on select\n");
            perror("select");
            exit(1);
        }
        printf("select done\n");

        for(int clientIndex = 0; clientIndex < FD_SETSIZE; clientIndex++) {
            if(FD_ISSET(clientIndex, &ReadySockets)) {
                if(clientIndex == sockfd) {

                    int client_socket = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addressLength);
                    if (client_socket < 0) {
                        printf("ERROR on accept\n");
                    }
                    printf("%d accept done\n", clientIndex);
                    FD_SET(client_socket, &CurrentSockets);
                } else {

                    printf("%d client\n", clientIndex);
                    char string[1024] = {0};  // Buffer to store input
                    int j = 0;
                    char buffer[1024];
                    while(1) {
                        int bytesRead = read(clientIndex, buffer, 1024);
        if (bytesRead < 0) {
            printf("ERROR on read\n");
            close(clientIndex);
            FD_CLR(clientIndex, &CurrentSockets);
            break;
        }
        buffer[bytesRead] = '\0';  // Null-terminate the received data

        // Process each received byte
        for (int u = 0; u < bytesRead; u++) {
            if (buffer[u] == '\n') {  // Enter key detected
                string[j] = '\0';    // Terminate the string
                printf("Full input: %s\n", string);

                // Check if input is "quit"
                if (
              string[0] == 'q' &&
              string[1] == 'u' &&
              string[2] == 'i' &&
              string[3] == 't') {
                    string[j] = '\0';  // Properly terminate the string
                    printf("Received quit command\n");
                    FD_CLR(clientIndex, &CurrentSockets);
                    break;  // or use another method to break out
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
                write(clientIndex, buffer, strlen(buffer) - 1);
                // Prevent buffer overflow
                if (j >= sizeof(string) - 1) {
                    j = 0;  // Reset or handle overflow
                }
            }
        }
        //memset(string, 0, sizeof(string));

        memset(buffer, 0, 1024);  // Clear the read buffer
                    }
                    FD_CLR(clientIndex, &CurrentSockets);
                }
            }
        }


    }

    exit_loop:

    close(sockfd);

    return 0;
}