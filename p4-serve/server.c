#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Trivial server
 * This listens on the port mentioned below.  When you run it on replit.com 
 * or on github codespaces with port forwarding, the port will get forwarded to
 * a "name" that you can then connect to using a browser.  
 * 
 * It returns a valid HTTP response, so it can be displayed on a browser.
 * 
 * You can modify the write template to send back any information you want.
 * You can also use the value from "read" function to decide what information
 * to send.
 */
#define PORT 12345
#define BACKLOG 5
#define BUFFER_SIZE 1024

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char recvbuf[BUFFER_SIZE];
    char sendbuf[BUFFER_SIZE];
    char sendtpl[] =
        "HTTP/1.0 200 OK\n"
        "Content-Type: text/html\n\n"
        "<h1>Hello, client!</h1>\n"
        "%d requests received so far.\n";
    int count = 0;

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, BACKLOG) == -1) {
        perror("Error listening for connections");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept a connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen)) == -1) {
            perror("Error accepting connection");
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted from %s\n", inet_ntoa(clientAddr.sin_addr));

        if (read(clientSocket, recvbuf, BUFFER_SIZE) < 0) {
            perror("Error reading request");
            close(clientSocket);
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        count++;
        sprintf(sendbuf, sendtpl, count);
        // Send a string to the client
        if (write(clientSocket, sendbuf, strlen(sendbuf)) < 0) {
            perror("Error sending data");
            close(serverSocket);
            close(clientSocket);
            exit(EXIT_FAILURE);
        }

        printf("Message sent to client: %s\n", sendbuf);
        close(clientSocket);
    }
    // Close the sockets
    close(serverSocket);

    return 0;
}
