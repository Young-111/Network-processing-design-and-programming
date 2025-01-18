#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Error handling function
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Signal handler to prevent zombie processes
void sigchld_handler(int signo)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Function to handle communication with a client
void handle_client(int client_sockfd)
{
    char buffer[256];
    int n;

    // Clear the buffer
    bzero(buffer, 256);

    // Read message from client
    n = read(client_sockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");

    printf("Here is the message: %s\n", buffer);

    // Send acknowledgment to client
    n = write(client_sockfd, "I got your message", 18);
    if (n < 0) error("ERROR writing to socket");

    // Close the client socket
    close(client_sockfd);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    // Set up signal handler for cleaning up child processes
    signal(SIGCHLD, sigchld_handler);

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    // Initialize server address structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // Listen for incoming connections
    listen(sockfd, 5);
    printf("Server listening on port %d...\n", portno);

    // Main loop to accept and handle client connections
    while (1) {
        clilen = sizeof(cli_addr);

        // Accept a new connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");

        // Create a new process to handle the client
        pid_t pid = fork();
        if (pid < 0) {
            error("ERROR on fork");
        } else if (pid == 0) {
            // Child process
            close(sockfd); // Close the listening socket in the child
            handle_client(newsockfd);
        } else {
            // Parent process
            close(newsockfd); // Close the client socket in the parent
        }
    }

    // Close the main socket
    close(sockfd);
    return 0;
}