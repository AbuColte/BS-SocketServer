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

#define PORT 4445
#define MAX_CLIENTS 10
#define MAX_PAIRS 100
#define MAX_KEY_LEN 50
#define MAX_VALUE_LEN 100


typedef struct {
    char subbedKey[MAX_KEY_LEN];
    char subbedValue[MAX_VALUE_LEN];
    int subbedClientID;
} SubbedData;

typedef struct {
    char key[MAX_PAIRS][MAX_KEY_LEN];
    char value[MAX_PAIRS][MAX_VALUE_LEN];
    int storageSize;
    SubbedData subbedData[20];
    int subbedDataIndex;
} KeyValueStorage;

typedef struct {
    char publicKey[MAX_KEY_LEN];
    char publicValue[MAX_VALUE_LEN];
    int publishedClientID;
} PublishedData;

typedef struct {
    KeyValueStorage storage[MAX_CLIENTS];
    int transactionInt;
    PublishedData publishedData[20];
    int publishedDataIndex;
} SharedMemoryData;

SharedMemoryData *sharedData;

int handle_client(int client_fd, int clientID) {
    sharedData->storage[clientID - 1].subbedDataIndex = 0;
    sharedData->storage[clientID - 1].storageSize = 0;
    char string[1024] = {0};
    int j = 0;
    char buffer[1024];
    char subBuffer[1024];
    char *subBufferPtr = subBuffer;
    while(1) {


        int bytesRead = read(client_fd, buffer, 1024);
        if (bytesRead < 0) {
            printf("ERROR on read\n");
            close(client_fd);
            break;
        }
        buffer[bytesRead] = '\0';


        for (int u = 0; u < bytesRead; u++) {
            if (sharedData->transactionInt != 0) {
                if(clientID != sharedData->transactionInt) {
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
                    sharedData->transactionInt = clientID;
                    memset(string, 0, sizeof(string));
                }

                if(string[0] == 'c' && string[1] == 'h' && string[2] == 'e' && string[3] == 'c' && string[4] == 'k') {
                    if (sharedData->storage[clientID - 1].subbedData[sharedData->storage[clientID - 1].subbedDataIndex].subbedKey != NULL) {
                        if (&subBufferPtr != NULL) {
                            if (subBufferPtr != sharedData->storage[clientID - 1].subbedData[sharedData->storage[clientID - 1].subbedDataIndex].subbedValue) {
                                strcpy(subBufferPtr, sharedData->storage[clientID - 1].subbedData[sharedData->storage[clientID - 1].subbedDataIndex].subbedValue);
                                printf("Public Key: %s\n", sharedData->publishedData[sharedData->publishedDataIndex - 1].publicKey);
                                printf("Public Key: %s\n", sharedData->publishedData[sharedData->publishedDataIndex - 1].publicValue);
                            }
                        }
                    }
                    memset(string, 0, sizeof(string));
                }

                if (string[0] == 's' && string[1] == 'u' && string[2] == 'b') {
                    char *command = strtok(string, " ");

                    if (command && strcmp(command, "sub") == 0) {
                        char *key = strtok(NULL, " ");
                        if (key) {
                            key[strlen(key) - 1] = '\0';
                            for (int i = 0; i < sharedData->publishedDataIndex; i++) {
                                if (strcmp(key, sharedData->publishedData[i].publicKey) == 0 && sharedData->publishedData[i].publishedClientID != clientID) {
                                    strcpy(sharedData->storage[clientID - 1].subbedData[sharedData->storage[clientID - 1].subbedDataIndex].subbedKey, key);
                                    strcpy(sharedData->storage[clientID - 1].subbedData[sharedData->storage[clientID - 1].subbedDataIndex].subbedValue, sharedData->publishedData[i].publicValue);
                                    sharedData->storage[clientID - 1].subbedData[sharedData->storage[clientID - 1].subbedDataIndex].subbedClientID =
                                        sharedData->publishedData[i].publishedClientID;
                                    sharedData->storage[clientID - 1].subbedDataIndex++;
                                    printf("Subbed Index: %d\n", sharedData->storage[clientID - 1].subbedDataIndex);
                                    printf("Subbed Key: %s\n", sharedData->storage[clientID - 1].subbedData[sharedData->storage[clientID - 1].subbedDataIndex].subbedKey);
                                }
                                printf("Public Key: %s\n", sharedData->publishedData[i].publicKey);
                                printf("Public Key: %s\n", sharedData->publishedData[i].publicValue);
                            }
                        } else {
                            printf("Error: key or value missing.\n");
                        }
                    } else {
                        printf("Not a 'put' command.\n");
                    }
                    memset(string, 0, sizeof(string));
                }

                if (string[0] == 'p' && string[1] == 'u' && string[2] == 'b') {
                    char *command = strtok(string, " ");

                    if (command && strcmp(command, "pub") == 0) {
                        char *key = strtok(NULL, " ");
                        if (key) {
                            key[strlen(key) - 1] = '\0';

                            for (int i = 0; i < sharedData->storage[clientID - 1].storageSize; i++) {
                                if (strcmp(sharedData->storage[clientID - 1].key[i], key) == 0) {
                                    printf("published key: %s\n", key);
                                    strcpy(sharedData->publishedData[sharedData->publishedDataIndex].publicKey, key);
                                    strcpy(sharedData->publishedData[sharedData->publishedDataIndex].publicValue, sharedData->storage[clientID - 1].value[i]);
                                    sharedData->publishedData[sharedData->publishedDataIndex].publishedClientID = clientID;
                                    sharedData->publishedDataIndex++;

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

                if(string[0] == 'e' && string[1] == 'n' && string[2] == 'd') {
                    if (sharedData->transactionInt != 0) {
                        sharedData->transactionInt = 0;
                    }
                    else {
                        printf("No Transaction Occuring");
                    }
                    memset(string, 0, sizeof(string));
                }

                // Check if input starts with "put"
                if (string[0] == 'p' && string[1] == 'u' && string[2] == 't') {
                    char *command = strtok(string, " ");
                    if (command && strcmp(command, "put") == 0) {
                        char *key = strtok(NULL, " ");
                        char *value = strtok(NULL, " ");
                        if (key && value) {
                            key[sizeof(key)] = '\0';
                            value[sizeof(key)] = '\0';
                            for (int i = 0; i <= sharedData->storage[clientID - 1].storageSize; i++) {
                                if (strcmp(sharedData->storage[clientID - 1].key[i], key) == 0) {
                                    strcpy(sharedData->storage[clientID - 1].value[i], value);
                                    printf("Changed Key: %s\n", key);
                                    printf("New Value: %s\n", value);
                                    for (int p = 0; p < sharedData->publishedDataIndex; p++) {
                                        if (strcmp(sharedData->publishedData[p].publicKey, key) == 0) {
                                            strcpy(sharedData->publishedData[p].publicValue, value);
                                            printf("changed Public Key: %s\n", key);
                                        }
                                    }
                                }
                                if (sharedData->storage[clientID - 1].storageSize == 0) {
                                    printf("Key: %s\n", key);
                                    printf("Value: %s\n", value);
                                    strcpy(sharedData->storage[clientID - 1].key[sharedData->storage[clientID - 1].storageSize], key);
                                    strcpy(sharedData->storage[clientID - 1].value[sharedData->storage[clientID - 1].storageSize], value);
                                    sharedData->storage[clientID - 1].storageSize++;
                                    //put(key, value);
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

                if (string[0] == 'g' && string[1] == 'e' && string[2] == 't') {
                    char *command = strtok(string, " ");

                    if (command && strcmp(command, "get") == 0) {
                        char *key = strtok(NULL, " ");
                        if (key) {
                            int keyValIndex = 0;
                            key[strlen(key) - 1] = '\0';
                            for (int i = 0; i < sharedData->storage[clientID - 1].storageSize; i++) {
                                if (strcmp(sharedData->storage[clientID - 1].key[i], key) == 0) {
                                    printf("Value: %s\n", sharedData->storage[clientID - 1].value[i]);
                                }
                            }
                        } else {
                            printf("Error: key or value missing.\n");
                        }
                    } else {
                        printf("Not a 'get' command.\n");
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
                            for (int i = 0; i < sharedData->storage[clientID - 1].storageSize; i++) {
                                if (strcmp(sharedData->storage[clientID - 1].key[i], key) == 0) {
                                    strcpy(sharedData->storage[clientID - 1].value[i], sharedData->storage[client_fd].value[i + 1]);
                                    strcpy(sharedData->storage[clientID - 1].key[i], sharedData->storage[client_fd].key[i + 1]);
                                    i == 0 ? sharedData->storage[clientID - 1].storageSize = 0 : sharedData->storage[clientID - 1].storageSize--;
                                }
                            }
                        } else {
                            printf("Error: key missing.\n");
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
    key_t key = 1231;
    int shmid = shmget(key, sizeof(SharedMemoryData), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    sharedData = (SharedMemoryData *)shmat(shmid, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    sharedData->transactionInt = 0;
    sharedData->publishedDataIndex = 0;

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
