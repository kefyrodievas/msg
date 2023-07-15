#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <sys/socket.h>


#define PORT 8080
#define ADDR "127.0.0.1"

void *receive(void *socket_fd);

int main(int argc, char const *argv[ ]) {
    // Creating a socket
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        // printf("Socket creation failed\n");
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ADDR, &address.sin_addr) <= 0) {
        // printf( "Invalid address/ Address not supported \n");
        perror("Invalid address\n");
        exit(EXIT_FAILURE);
    }

    // Connecting to server
    int status;
    status = connect(socket_fd, (struct sockaddr *)&address, sizeof(address));
    if (status < 0) {
        // printf("\nConnection Failed \n");
        perror("Connection Failed\n");
        exit(EXIT_FAILURE);
    }

    char buffer[4096];

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive, (void *)&socket_fd);
    pthread_join(receive_thread, NULL);

    char *hello = "Hello!";
    send(socket_fd, hello, strlen(hello), 0);

    while (1) {
        memset(buffer, 0, 4096);
        scanf("%s\n", buffer);
        // fgets(buffer, sizeof(buffer), stdin);
        printf("%s\n", buffer);
        send(socket_fd, buffer, strlen(buffer), 0);
    }

    // Close the connected socket
    close(socket_fd);
    return 0;
}

void *receive(void *socket) {
    int socket_fd = *(int *)socket;
    char buffer[4096];
    memset(buffer, 0, 4096);

    while (1) {
        int bytesRecv = recv(socket_fd, buffer, 4096, 0);

        if (bytesRecv < 0) {
            perror("Connection issue\n");
            break;
        }

        if (bytesRecv == 0) {
            perror("Server disconnected\n");
            break;
        }

        // Display message
        printf("%s\n", buffer);
    }
    return NULL;
}