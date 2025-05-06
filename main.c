#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int put(char* key, char* value, int c) {
    printf("%s %s\n", key, value);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("ERROR opening socket\n");
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(1234);

    char buffer[1024];
    int addressLength = sizeof(address);

    bind(sockfd, (struct sockaddr*)&address, addressLength);
    listen(sockfd, 5);
    printf("Socket server auf port %d", ntohs(address.sin_port));

    int client_socket = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addressLength);
    if (client_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    send(client_socket, "TESTst\n", 13, 0);

    // while (1) {
    //     memset(buffer, 0, 1024);
    //     int bytes_read = read(client_socket, buffer, 1024);
    //     if (bytes_read <= 0) break;
    //
    //     buffer[strcspn(buffer, "\r\n")] = 0;
    //
    //     char command[10], key[100], value[100];
    //     int matched = sscanf(buffer, "%s %s %[^\n]", command, key, value);
    //
    //     if (matched >= 1 && strcmp(command, "PUT") == 0) {
    //         if (matched == 3) {
    //             put(key, value, client_socket);
    //         } else {
    //             send(client_socket, "ERROR Usage: PUT <key> <value>\n", 31, 0);
    //         }
    //     } else if (strcmp(buffer, "exit") == 0) {
    //         send(client_socket, "Bye!\n", 5, 0);
    //         break;
    //     } else {
    //         send(client_socket, "ERROR Unknown command\n", 23, 0);
    //     }
    // }
    return 0;
}
