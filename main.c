#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 4447
#define MAX_CLIENTS 10
#define MAX_PAIRS 100
#define MAX_KEY_LEN 50
#define MAX_VALUE_LEN 100

typedef struct {
    char key[50][1024];
    char value[50][1024];
} KeyValueStorage;

KeyValueStorage storage[50];

int transactionInt;
int *transactionIntPtr = &transactionInt;

int handle_client(int client_fd, int clientID) {
    int storageSize = 0;
    char string[1024] = {0};
                    int j = 0;
                    char buffer[1024];
                    while(1) {
                        printf("%d\n", clientID);


                        int bytesRead = read(client_fd, buffer, 1024);
        if (bytesRead < 0) {
            printf("ERROR on read\n");
            close(client_fd);
            break;
        }
        buffer[bytesRead] = '\0';


        for (int u = 0; u < bytesRead; u++) {
            if (*transactionIntPtr != 0) {
                if(clientID != *transactionIntPtr) {
                    send(client_fd, "transaction occuring...please wait\n", 35, 0);
                    continue;
                }
            }
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
                    return 0;
              }

                if(string[0] == 'b' && string[1] == 'e' && string[2] == 'g') {
                    *transactionIntPtr = clientID;
                    memset(string, 0, sizeof(string));
                }

                if(string[0] == 't' && string[1] == 'e' && string[2] == 's') {
                    if(*transactionIntPtr == clientID) {
                        printf("JAckemann");
                        printf("Current Transaction %d\n", *transactionIntPtr);
                    } else {
                        printf("nackenmann");
                        printf("Current Transaction %d\n", *transactionIntPtr);
                    }
                }

                if(string[0] == 'e' && string[1] == 'n' && string[2] == 'd') {
                    if (*transactionIntPtr != 0) {
                        *transactionIntPtr = 0;
                    }
                    else {
                        printf("No Transaction Occuring");
                    }
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
                            strcpy(storage[clientID - 1].key[storageSize], key);
                            storage[clientID - 1].key[storageSize][sizeof(storage[client_fd].key[storageSize]) - 1] = '\0';
                            strcpy(storage[clientID - 1].value[storageSize], value);
                            storage[clientID - 1].value[storageSize][sizeof(storage[client_fd].value[storageSize]) - 1] = '\0';
                            storageSize++;
                            //put(key, value);
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
                        if (key) {
                            printf("At Key: %s\n", key);
                            int keyValIndex = 0;
                            key[strlen(key) - 1] = '\0';
                            for (int i = 0; i < storageSize; i++) {
                                if (strcmp(storage[clientID - 1].key[i], key) == 0) {
                                    printf("%s\n", storage[clientID - 1].value[i]);
                                }
                            }
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
                        if (key) {
                            key[strlen(key) - 1] = '\0';
                            printf("Key deleted: %s\n", key);
                            for (int i = 0; i < storageSize; i++) {
                                if (strcmp(storage[clientID - 1].key[i], key) == 0) {
                                    strcpy(storage[clientID - 1].value[i], storage[client_fd].value[i + 1]);
                                    strcpy(storage[clientID - 1].key[i], storage[client_fd].key[i + 1]);
                                    i == 0 ? storageSize = 0 : storageSize--;
                                }
                            }
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
                write(client_fd, buffer, strlen(buffer));
                // Prevent buffer overflow
                if (j >= sizeof(string) - 1) {
                    j = 0;  // Reset or handle overflow
                }
            }
        }
        //memset(string, 0, sizeof(string));

        memset(buffer, 0, 1024);  // Clear the read buffer
                    }

    close(client_fd);
    exit(0);
}

int ClientCount = 0;

int main() {
    signal(SIGCHLD, SIG_IGN);  // Prevent zombies

    // Shared memory setup
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    transactionIntPtr = (int *) shmat(shmid, NULL, 0);
    if (transactionIntPtr == (void *) -1) {
        perror("shmat failed");
        exit(1);
    }


    *transactionIntPtr = 0;  // initialize transactionInt to 0

    // Socket setup
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) ? printf("failed") : printf("success");
    listen(sockfd, MAX_CLIENTS);

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t addr_size = sizeof(clientAddr);
        int newSocket = accept(sockfd, (struct sockaddr *)&clientAddr, &addr_size);
        printf("Accepted new connection, fd=%d\n", newSocket);
        ClientCount++;

        if (fork() == 0) {
            close(sockfd);  // child doesn't need listening socket
            printf("Client %d'th connected\n", ClientCount);
            handle_client(newSocket, ClientCount);
        } else {
            close(newSocket);  // parent doesn't handle client
        }
    }

    // Cleanup
    close(sockfd);

    return 0;
}
